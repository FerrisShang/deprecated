#include <pthread.h>
#include "wi_bus.h"
#include "proc_comm.h"
#include "string.h"

#define IPC_SHM_KEY ((key_t)0X005FC0DE)
#define IPC_SEM_KEY ((key_t)0X005FC0DE)
#define SHMGET_SIZE 1800

struct wi_bus_data wi_bus;

static bool is_broadcast_cb(void *id, int id_len)
{
	const static char bc_addr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	if(!memcmp(bc_addr, id+1, sizeof(wiaddr_t)-1)){
		return true;
	}else{
		return false;
	}
}
static bool is_bc_match_cb(void *id_bc, void *id, int id_len)
{
	if(*(char*)id == 0xFF || *(char*)id == *(char*)id_bc){
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
		Log.d("wi_bus server closed");
		mem_dump();
		return 0;
	}else{
		return -1;
		Log.e("create wi_bus server failed");
	}
}

struct wi_bus_data {
	struct pc_c_client *client;
	void (*recv_cb)(wiaddr_t *remote_id, char *buf, int len, void *user_data);
	void (*disconnect_cb)(void *user_data);
	void *pdata;
};
static void* wi_bus_client_run(void *pdata)
{
	struct wi_bus_data *wi_bus = pdata;
	pc_client_run(wi_bus->client);
	Log.d("wi_bus disconnected");
	wi_bus->disconnect_cb(wi_bus->pdata);
	return NULL;
}

static void wi_bus_recv_cb(void *id, int id_len, void *buf, int buf_len, void *pdata)
{


	Log.v("length:%d", buf_len);
	Log.v("data:%s", hex2str(buf, 32));
	Log.v("     %s", hex2str(buf+32, 32));
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
			(void*)local_id, sizeof(wiaddr_t), wi_bus_recv_cb, user_data);
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
