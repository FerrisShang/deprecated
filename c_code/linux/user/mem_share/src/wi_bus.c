#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "wi_bus.h"
#include "proc_comm.h"
#include "queue.h"
#include "wi_mutex.h"
#include "wi_sem.h"
#include "types.h"

#define IPC_SHM_KEY ((key_t)0X005FC0DE)
#define IPC_SEM_KEY ((key_t)0X005FC0DE)
#define SHMGET_SIZE WI_BUS_MAX_BUFFER_SIZE

#define SEND_NONBLOCK 1

#if SEND_NONBLOCK == 1

struct wi_bus_list_data{
	struct wi_bus_data *wi_bus;
	long thread_num;
};

struct recv_data {
	wiaddr_t remote_id;
	char *buf;
	int len;
};

struct wi_bus_data {
	struct pc_c_client *client;
	void (*recv_cb)(wiaddr_t *remote_id, char *buf, int len, void *user_data);
	void (*disconnect_cb)(void *user_data);
	void *pdata;
	pthread_t thread_run;
	pthread_t thread_read;
	//data field
	struct queue *recv_list; /* type of struct recv_data */
	wi_mutex_t mutex;
	wi_sem_t sem;
};

struct queue *wi_bus_list;

static bool is_thread_match(const void *data, const void *match_data)
{
	const struct wi_bus_list_data *list = data;
	long thread_num = syscall(4222);
	return (list->thread_num == thread_num);
}
static struct wi_bus_data* get_current_wi_bus(void)
{
	struct wi_bus_list_data *list_data;
	if(!wi_bus_list){
		return NULL;
	}
	list_data = queue_find(wi_bus_list, is_thread_match, NULL);
	if(!list_data){
		return NULL;
	}
	return list_data->wi_bus;
}
static bool is_broadcast_cb(void *id, int id_len)
{
	const static char bc_addr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	if(!memcmp(bc_addr, id+2, sizeof(wiaddr_t)-2)){
		return true;
	}else{
		return false;
	}
}
static bool is_bc_match_cb(void *id_bc, void *id, int id_len)
{
	if((*(char*)id == *(char*)id_bc && *(char*)(id+1) == *(char*)(id_bc+1)) ||
			(*(char*)id_bc== 0xff && *(char*)(id_bc+1) == 0xff)){
		return true;
	}else{
		return false;
	}
}
int wi_bus_server_run(void)
{
	struct pc_server *server;
	server = pc_create_server(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
			is_broadcast_cb, is_bc_match_cb);
	if(server){
		pc_server_run(server);
		pc_destroy_server(server);
		Log.d("wi_bus server closed");
		mem_dump();
		exit(0);
		return 0;
	}else{
		return -1;
		Log.e("create wi_bus server failed");
	}
}

static void* wi_bus_client_run(void *pdata)
{
	struct wi_bus_data *wi_bus = pdata;
	if(!wi_bus){
		Log.e("wi_bus not registed");
		return NULL;
	}
	pc_client_run(wi_bus->client);
	if(wi_bus->disconnect_cb){
		wi_bus->disconnect_cb(wi_bus->pdata);
	}
	return NULL;
}

static void* wi_bus_client_read(void *pdata)
{
	struct wi_bus_data *wi_bus = pdata;
	struct recv_data *buf_data;
	if(!wi_bus){
		Log.e("wi_bus not registed");
		return NULL;
	}
	while(1){
		wi_sem_wait(&wi_bus->sem);
		while(1){
			wi_mutex_lock(&wi_bus->mutex);
			buf_data = queue_pop_head(wi_bus->recv_list);
			wi_mutex_unlock(&wi_bus->mutex);
			if(buf_data){
				wi_bus->recv_cb(&buf_data->remote_id,
						buf_data->buf, buf_data->len, wi_bus->pdata);
				wi_mutex_lock(&wi_bus->mutex);
				mem_free(buf_data->buf);
				mem_free(buf_data);
				wi_mutex_unlock(&wi_bus->mutex);
			}else{
				break;
			}
		}
	}
	return NULL;
}

static void wi_bus_recv_cb(void *id, int id_len, void *buf, int buf_len, void *pdata)
{
	struct wi_bus_data *wi_bus = pdata;
	UINT8 *p = buf;
	UINT32 flag;
	struct recv_data *data;
	data = mem_malloc(sizeof(struct recv_data));
	if(!data){
		goto malloc_data_failed;
	}
	data->buf = mem_malloc(buf_len);
	if(!data->buf){
		goto malloc_buf_failed;
	}
	STREAM_TO_UINT32(flag, p);
	data->len = buf_len - sizeof(UINT32);
	memcpy(data->buf, p, data->len);
	memcpy(data->remote_id.d, id, id_len);
	wi_mutex_lock(&wi_bus->mutex);
	if(flag & WI_FLAG_PRIORITY_HIGH){
		queue_push_head(wi_bus->recv_list, data);
	}else{
		queue_push_tail(wi_bus->recv_list, data);
	}
	wi_mutex_unlock(&wi_bus->mutex);
	wi_sem_post(&wi_bus->sem);
	return;
malloc_buf_failed :
	mem_free(data);
malloc_data_failed :
	Log.e("malloc failed");
	return;
}
int wi_register(
		wiaddr_t *local_id,
		void (*recv_cb)(wiaddr_t *remote_id, char *buf, int len, void *user_data),
		void (*disc_cb)(void *user_data),
		void *user_data)
{
	struct wi_bus_data *wi_bus;
	struct pc_c_client *client;
	struct wi_bus_list_data *list_data;
	if(!wi_bus_list){
		wi_bus_list = queue_new();
		if(!wi_bus_list){
			goto queue_new_wi_bus_failed;
		}
	}
	list_data = mem_malloc(sizeof(struct wi_bus_list_data));
	if(!list_data){
		goto malloc_list_data_failed;
	}
	wi_bus = mem_malloc(sizeof(struct wi_bus_data));
	if(!wi_bus){
		goto malloc_wi_bus_failed;
	}
	wi_bus->recv_list = queue_new();
	if(!wi_bus->recv_list){
		goto queue_new_failed;
	}
	if(wi_mutex_init(&wi_bus->mutex)<0){
		goto wi_mutex_init_failed;
	}
	if(wi_sem_init(&wi_bus->sem, 0)<0){
		goto wi_sem_init_failed;
	}
	client = pc_req_create_client(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
			(void*)local_id, sizeof(wiaddr_t), is_broadcast_cb,
			wi_bus_recv_cb, wi_bus);
	if(!client){
		Log.e("wi_bus register failed");
		goto create_client_failed;
	}
	wi_bus->client = client;
	wi_bus->recv_cb = recv_cb;
	wi_bus->disconnect_cb = disc_cb;
	wi_bus->pdata = user_data;
	pthread_create(&wi_bus->thread_run, NULL, wi_bus_client_run, wi_bus);
	pthread_create(&wi_bus->thread_read, NULL, wi_bus_client_read, wi_bus);

	list_data->wi_bus = wi_bus;
	list_data->thread_num = syscall(4222);
	queue_push_tail(wi_bus_list, list_data);

	return WI_RET_SUCCESS;
create_client_failed :
	wi_sem_destroy(&wi_bus->sem);
wi_sem_init_failed :
	wi_mutex_destroy(&wi_bus->mutex);
wi_mutex_init_failed :
	queue_destroy(wi_bus->recv_list, NULL);
queue_new_failed :
	mem_free(wi_bus);
malloc_wi_bus_failed :
	mem_free(list_data);
malloc_list_data_failed :
queue_new_wi_bus_failed :
	return WI_RET_FAILED;
}

struct wi_send_data {
	char *buf;
	int len;
	int flag;
	int res;
};
static void out_cb(void *buf, int *buf_len, void *pdata)
{
	struct wi_send_data *wi_send = pdata;
	UINT8 *p = buf;
	UINT32_TO_STREAM(p, wi_send->flag);
	ARRAY_TO_STREAM(p, wi_send->buf, wi_send->len);
	*buf_len = p - (UINT8*)buf;
}
static void ret_cb(int status, void *pdata)
{
	struct wi_send_data *wi_send = pdata;
	wi_send->res = status;
}
int wi_send(wiaddr_t *remote_id, char *buf, int len, int flag)
{
	struct wi_send_data wi_send;
	struct wi_bus_data *wi_bus;
	int res;
	wi_bus = get_current_wi_bus();
	if(!wi_bus || !wi_bus->client){
		Log.e("wi_bus not registered");
		return -1;
	}
	wi_send.buf = buf;
	wi_send.len = len;
	wi_send.flag = flag;
	res = pc_c_send(wi_bus->client, (char*)remote_id, sizeof(wiaddr_t),
			out_cb, ret_cb, &wi_send);
	if(res < 0){
		return res;
	}
	return wi_send.res;
}
static void free_recv_data(void *data)
{
	struct recv_data *recv = data;
	mem_free(recv->buf);
	mem_free(recv);
}
int wi_unregister(void)
{
	struct wi_bus_data *wi_bus;
	wi_bus = get_current_wi_bus();
	if(!wi_bus || !wi_bus->client || !wi_bus->recv_list){
		return -1;
	}
	queue_remove_if(wi_bus_list, is_thread_match, NULL);
	pc_req_destroy_client(wi_bus->client);
	wi_sem_destroy(&wi_bus->sem);
	wi_mutex_destroy(&wi_bus->mutex);
	queue_destroy(wi_bus->recv_list, free_recv_data);
	mem_free(wi_bus);
	return 0;
}
#else /* SEND_BLOCK */
/*
struct wi_bus_data {
	struct pc_c_client *client;
	void (*recv_cb)(wiaddr_t *remote_id, char *buf, int len, void *user_data);
	void (*disconnect_cb)(void *user_data);
	void *pdata;
};
struct wi_bus_data wi_bus;

static bool is_broadcast_cb(void *id, int id_len)
{
	const static char bc_addr[] = "ffffff";//{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	if(!memcmp(bc_addr, id+2, sizeof(wiaddr_t)-2)){
		return true;
	}else{
		return false;
	}
}
static bool is_bc_match_cb(void *id_bc, void *id, int id_len)
{
	if((*(char*)id == *(char*)id_bc && *(char*)(id+1) == *(char*)(id_bc+1)) ||
			(*(char*)id_bc== 'f' && *(char*)(id_bc+1) == 'f')){
		return true;
	}else{
		return false;
	}
}
int wi_bus_server_run(void)
{
	struct pc_server *server;
	server = pc_create_server(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
			is_broadcast_cb, is_bc_match_cb);
	if(server){
		pc_server_run(server);
		pc_destroy_server(server);
		Log.d("wi_bus server closed");
		mem_dump();
		exit(0);
		return 0;
	}else{
		return -1;
		Log.e("create wi_bus server failed");
	}
}

static void* wi_bus_client_run(void *pdata)
{
	struct wi_bus_data *wi_bus = pdata;
	if(!wi_bus){
		Log.e("wi_bus not registed");
		return NULL;
	}
	pc_client_run(wi_bus->client);
	wi_bus->disconnect_cb(wi_bus->pdata);
	return NULL;
}

static void wi_bus_recv_cb(void *id, int id_len, void *buf, int buf_len, void *pdata)
{
	wi_bus.recv_cb((wiaddr_t*)id, buf, buf_len, pdata);
}
int wi_register(
		wiaddr_t *local_id,
		void (*recv_cb)(wiaddr_t *remote_id, char *buf, int len, void *user_data),
		void (*disc_cb)(void *user_data),
		void *user_data)
{
	struct pc_c_client *client;
	pthread_t client_t;
	client = pc_req_create_client(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
			(void*)local_id, sizeof(wiaddr_t), is_broadcast_cb,
			wi_bus_recv_cb, user_data);
	if(!client){
		Log.e("wi_bus register failed");
		return WI_RET_FAILED;
	}
	wi_bus.client = client;
	wi_bus.recv_cb = recv_cb;
	wi_bus.disconnect_cb = disc_cb;
	wi_bus.pdata = user_data;
	pthread_create(&client_t, NULL, wi_bus_client_run, &wi_bus);
	return WI_RET_SUCCESS;
}

struct wi_send_data {
	char *buf;
	int len;
	int flag;
	int res;
};
static void out_cb(void *buf, int *buf_len, void *pdata)
{
	struct wi_send_data *wi_send = pdata;
	*buf_len = wi_send->len;
	memcpy(buf, wi_send->buf, wi_send->len);
}
static void ret_cb(int status, void *pdata)
{
	struct wi_send_data *wi_send = pdata;
	wi_send->res = status;
}
int wi_send(wiaddr_t *remote_id, char *buf, int len, int flag)
{
	struct wi_send_data wi_send;
	if(!wi_bus.client){
		Log.e("wi_bus not registered");
	}
	wi_send.buf = buf;
	wi_send.len = len;
	wi_send.flag = flag;
	pc_c_send(wi_bus.client, (char*)remote_id, sizeof(wiaddr_t),
			out_cb, ret_cb, &wi_send);
	return wi_send.res;
}
int wi_unregister(void)
{
	if(!wi_bus.client){
		return -1;
	}
	pc_req_destroy_client(wi_bus.client);
	memset(&wi_bus, 0, sizeof(wi_bus));
	return 0;
}
*/
#endif /* SEND_NONBLOCK */
