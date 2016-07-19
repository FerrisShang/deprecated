#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "proc_comm.h"
#include "types.h"

/*
 * format:
 *     command | data
 * command | data:
 *     req: CMD_GET_SERVER_REQ  | null
 *     rsp: CMD_GET_SERVER_RSP  | result
 *     req: CMD_FREE_SERVER_REQ | null
 *     rsp: CMD_FREE_SERVER_RSP | null
 *     req: CMD_SEND_DATA_REQ   | null
 *     rsp: CMD_SEND_DATA_RSP   | null
 *
 *
 *
 */

enum{
	CMD_SUCCESS = 0,
	CMD_FAILED = -1,
};
enum{
	CMD_GET_SERVER_REQ = 0,
	CMD_GET_SERVER_RSP,
	CMD_CLOSE_SERVER_REQ,
	CMD_CLOSE_SERVER_RSP,
	CMD_DATA_SEND_REQ,
	CMD_DATA_SEND_RSP,
};
#define RESERVE_SIZE (MAX_CLIENT_ID_LEN*2+sizeof(int)*2)

struct pc_server {
	struct ipc_cmd_local *cmd;
	pc_recv_cb recv_cb;
	pc_change_req_cb conn_state_change_req;
	void *pdata;
	//client data
	struct queue *client_list; /* struct pc_s_client */
};
struct pc_c_client {
	struct ipc_cmd_remote *cmd_l;
	struct ipc_cmd_remote *cmd_r;
	struct client_id *id;
	pc_recv_cb recv_cb;
	void *pdata;
};
struct pc_s_client {
	struct ipc_cmd_local *client_cmd;
	struct client_id *id;
};

static struct pc_s_client* create_remote_server(
		struct pc_server* server, int buffer_size, void *id, int id_len);
static int destroy_remote_server(struct pc_server* server ,void *id, int id_len);
static int list_add_client(struct queue *list, struct pc_s_client *client);
static struct pc_s_client* list_find_client(
		struct queue *list, void *id, int id_len);
static int list_remove_client(struct queue *list, struct pc_s_client *client);

struct pc_server* pc_create_server(key_t sem_key, key_t shm_key,
		int buffer_size,
		pc_recv_cb recv_cb,
		pc_change_req_cb conn_state_change_req,
		void *pdata)
{
	struct pc_server* server;
	server = mem_malloc(sizeof(struct pc_server));
	if(!server){
		goto malloc_server_failed;
	}
	server->client_list = queue_new();
	if(!server->client_list){
		goto queue_new_failed;
	}
	server->pdata = pdata;
	server->recv_cb = recv_cb;
	server->conn_state_change_req = conn_state_change_req;
	server->cmd = ipc_create_cmd(sem_key, shm_key, RESERVE_SIZE + buffer_size);
	if(!server->cmd){
		goto ipc_create_cmd_failed;
	}
	return server;
ipc_create_cmd_failed :
	queue_destroy(server->client_list, NULL);
queue_new_failed :
	mem_free(server);
malloc_server_failed :
	return NULL;
}

static struct pc_s_client* create_remote_server(
		struct pc_server* server, int buffer_size, void *id, int id_len)
{
	struct pc_s_client* client;
	client = list_find_client(server->client_list, id, id_len);
	if(client){
		Log.e("client id exsit");
		return NULL;
	}
	if(id_len > MAX_CLIENT_ID_LEN){
		Log.e("client id too long");
		return NULL;
	}
	client = mem_malloc(sizeof(struct pc_s_client));
	if(!client){
		goto malloc_client_failed;
	}
	client->id = mem_malloc(sizeof(struct client_id));
	if(!client->id){
		goto malloc_c_data_failed;
	}
	memcpy(client->id->data, id, id_len);
	client->id->len = id_len;
	client->client_cmd = ipc_create_cmd(IPC_PRIVATE, IPC_PRIVATE,
			RESERVE_SIZE + buffer_size);
	if(!client->client_cmd){
		Log.e("create client failed");
		goto ipc_create_cmd_failed;
	}
	if(list_add_client(server->client_list, client) < 0){
		Log.e("add new client to list failed");
		goto add_client_failed;
	}
	return client;
add_client_failed :
	ipc_destroy_local_cmd(client->client_cmd);
ipc_create_cmd_failed :
	mem_free(client->id);
malloc_c_data_failed :
	mem_free(client);
malloc_client_failed :
	return NULL;
}

static int destroy_remote_server(struct pc_server* server ,void *id, int id_len)
{
	struct pc_s_client *client;
	client = list_find_client(server->client_list, id, id_len);
	if(!client){
		return -1;
	}
	list_remove_client(server->client_list, client);
	ipc_destroy_local_cmd(client->client_cmd);
	mem_free(client->id);
	mem_free(client);
	return 0;
}
#define REQ_OFFSET_CMD       0
#define REQ_OFFSET_ID_LEN    sizeof(int)
#define REQ_OFFSET_ID        (REQ_OFFSET_ID_LEN + sizeof(int))
#define REQ_OFFSET_MEM_SIZE  (REQ_OFFSET_ID + MAX_CLIENT_ID_LEN)
#define REQ_OFFSET_S_ID      REQ_OFFSET_ID
#define REQ_OFFSET_D_ID      (OFFSET_S_ID + MAX_CLIENT_ID_LEN)
#define REQ_OFFSET_DATA_LEN  (OFFSET_D_ID + MAX_CLIENT_ID_LEN)
#define REQ_OFFSET_DATA      (OFFSET_DATA_LEN + sizeof(int))

static void pc_server_recv_cb(void *buf, void *pdata)
{
	struct pc_server* server = pdata;
	int res;
	UINT8 *p;
	UINT32 cmd;
	p = buf;
	STREAM_TO_UINT32(cmd, p);
	switch(cmd){
		case CMD_GET_SERVER_REQ :{// | cmd | id_len | id | mem_size |
			struct pc_s_client *pc_s_client;
			void *id;
			UINT32 mem_size, id_len;
			int i;//test
			for(i=0;i<128;i++){
				if((i%16)==0)printf("\n");
				printf("%02x ", *(UINT8*)(buf+i));
			}
			printf("    recevie done.\n");//test done
			STREAM_TO_UINT32(id_len, p);
			id = p;
			p += MAX_CLIENT_ID_LEN;
			STREAM_TO_UINT32(mem_size, p);
			pc_s_client = create_remote_server(server, mem_size, id, id_len);
			if(!pc_s_client){
				p = buf;
				UINT32_TO_STREAM(p, CMD_GET_SERVER_RSP);
				UINT32_TO_STREAM(p, CMD_FAILED);
				return;
			}

			/* | cmd | state | semid | size | shmid | size | */
			p = buf;
			UINT32_TO_STREAM(p, CMD_GET_SERVER_RSP);
			UINT32_TO_STREAM(p, CMD_SUCCESS);
			UINT32_TO_STREAM(p, pc_s_client->client_cmd->sem->semid);
			UINT32_TO_STREAM(p, pc_s_client->client_cmd->sem->nsems);
			UINT32_TO_STREAM(p, pc_s_client->client_cmd->shm->shmid);
			UINT32_TO_STREAM(p, pc_s_client->client_cmd->shm->size);

			for(i=0;i<128;i++){
				if((i%16)==0)printf("\n");
				printf("%02x ", *(UINT8*)(buf+i));
			}
			printf("    send done.\n");
			return;
			}break;
		case CMD_CLOSE_SERVER_REQ:{ // | cmd | id_len | id |
			void *id;
			UINT32 id_len;
			STREAM_TO_UINT32(id_len, p);
			id = p;
			res = destroy_remote_server(server, id, id_len);
			p = buf;
			if(res < 0){
				p = buf;
				UINT32_TO_STREAM(p, CMD_CLOSE_SERVER_RSP);
				UINT32_TO_STREAM(p, CMD_FAILED);
				return;
			}else{
				/* | cmd | state | */
				p = buf;
				UINT32_TO_STREAM(p, CMD_CLOSE_SERVER_RSP);
				UINT32_TO_STREAM(p, CMD_SUCCESS);
			}
			}break;
		case CMD_DATA_SEND_REQ :{// | cmd | id_len | s_id | d_id | d_len | data |
			void *s_id, *d_id;
			UINT32 id_len, data_len;
			UINT8 *data;
			STREAM_TO_UINT32(id_len, p);
			s_id = p;
			p += MAX_CLIENT_ID_LEN;
			d_id = p;
			p += MAX_CLIENT_ID_LEN;
			STREAM_TO_UINT32(data_len, p);
			data = p;
			//unfinished
			p = buf;
			}break;
		default :
			break;
	}
	return;
}
int pc_server_run(struct pc_server *server)
{
	int res;
	if(!server || !server->cmd){
		return -1;
	}
	while(1){
		res = ipc_recv_cmd(server->cmd, pc_server_recv_cb, server);
		if(res < 0){
			return res;
		}
	}
}

void client_list_destroy_cb(void *data)
{
	struct pc_s_client *client = data;
	ipc_destroy_local_cmd(client->client_cmd);
	mem_free(client->id);
}
int pc_destroy_server(struct pc_server* server)
{
	if(!server){
		return -1;
	}
	if(server->cmd){
		ipc_destroy_local_cmd(server->cmd);
	}
	queue_destroy(server->client_list, client_list_destroy_cb);
	mem_free(server);
	return 0;
}

int pc_destroy_client(struct pc_s_client* client)
{
	/*
	if(!client){
		return -1;
	}
	if(client->id){
		mem_free(client->id);
	}
	if(client->cmd_l){
		ipc_destroy_cmd(client->cmd_l);
	}
	if(client->cmd_r){
		ipc_destroy_cmd(client->cmd_r);
	}
	mem_free(client);
	*/
	return 0;
}

static void pc_req_create_client_req_cb(void *buf, void *pdata)
{
	struct pc_c_client *client = pdata;
	UINT8 *p = buf;
	UINT8 client_id[MAX_CLIENT_ID_LEN] = {0};
	/* | cmd | id_len | id | mem_size | */
	UINT32_TO_STREAM(p, CMD_GET_SERVER_REQ);
	UINT32_TO_STREAM(p, client->id->len);
	memcpy(client_id, client->id->data, client->id->len);
	ARRAY_TO_STREAM(p, client_id, MAX_CLIENT_ID_LEN);
	UINT32_TO_STREAM(p, client->cmd_r->shm->size);
}
static void pc_req_create_client_rsp_cb(void *buf, void *pdata)
{
	/* | cmd | state | semid | size | shmid | size | */
	struct pc_c_client *client = pdata;
	int cmd, state;
			int i;//for test
			for(i=0;i<128;i++){
				if((i%16)==0)printf("\n");
				printf("%02x ", *(UINT8*)(buf+i));
			}
			printf("\n");

	UINT8 *p = buf;
	STREAM_TO_UINT32(cmd, p);
	STREAM_TO_UINT32(state, p);
	if(cmd != CMD_GET_SERVER_RSP || state != CMD_SUCCESS){
		Log.e("request client failed");
		return;
	}
	STREAM_TO_UINT32(client->cmd_l->sem->semid, p);
	STREAM_TO_UINT32(client->cmd_l->sem->nsems, p);
	STREAM_TO_UINT32(client->cmd_l->shm->shmid, p);
	STREAM_TO_UINT32(client->cmd_l->shm->size, p);
}
struct pc_c_client* pc_req_create_client(key_t sem_key, key_t shm_key,
		int buffer_size,
		void *client_id, int id_len,
		pc_recv_cb callback, void *pdata)
{
	int res;
	struct pc_c_client *client;
	if(id_len == 0 || id_len > MAX_CLIENT_ID_LEN){
		Log.e("Invailed id length");
		return NULL;
	}
	client = mem_malloc(sizeof(struct pc_c_client));
	if(!client){
		goto malloc_client_failed;
	}
	client->cmd_r = mem_malloc(sizeof(struct ipc_cmd_remote));
	if(!client->cmd_r){
		goto malloc_cmd_r_failed;
	}
	client->cmd_r->sem = mem_malloc(sizeof(struct ipc_sem));
	if(!client->cmd_r->sem){
		goto malloc_sem_r_failed;
	}
	client->cmd_r->shm = mem_malloc(sizeof(struct ipc_shm));
	if(!client->cmd_r->shm){
		goto malloc_shm_r_failed;
	}
	client->cmd_l = mem_malloc(sizeof(struct ipc_cmd_local));
	if(!client->cmd_l){
		goto malloc_cmd_l_failed;
	}
	client->cmd_l->sem = mem_malloc(sizeof(struct ipc_sem));
	if(!client->cmd_l->sem){
		goto malloc_sem_l_failed;
	}
	client->cmd_l->shm = mem_malloc(sizeof(struct ipc_shm));
	if(!client->cmd_l->shm){
		goto malloc_shm_l_failed;
	}
	client->id = mem_malloc(sizeof(struct client_id));
	if(!client->id){
		goto malloc_id_failed;
	}
	//init remote server
	client->pdata = pdata;
	client->cmd_r->sem->key = sem_key;
	client->cmd_r->shm->key = shm_key;
	client->cmd_r->shm->size = RESERVE_SIZE + buffer_size;
	res = ipc_cmd_remote_init(client->cmd_r);
	if(res < 0){
		goto ipc_cmd_r_init_failed;
	}
	//request local server
	memset(client->cmd_l->sem, 0, sizeof(struct ipc_sem));
	memset(client->cmd_l->shm, 0, sizeof(struct ipc_shm));
	memcpy(client->id->data, client_id, id_len);
	client->id->len = id_len;
	client->recv_cb = callback;
	client->pdata = pdata;
	res = ipc_send_cmd(client->cmd_r,
			pc_req_create_client_req_cb, pc_req_create_client_rsp_cb, client);
	if(res < 0 ){
		goto request_local_server_failed;
	}
	res = ipc_cmd_remote_init(client->cmd_l);
	if(res < 0 ){
		goto request_local_server_failed;
	}
	return client;
request_local_server_failed :
ipc_cmd_r_init_failed :
	mem_free(client->id);
malloc_id_failed :
	mem_free(client->cmd_l->shm);
malloc_shm_l_failed :
	mem_free(client->cmd_l->sem);
malloc_sem_l_failed :
	mem_free(client->cmd_l);
malloc_cmd_l_failed :
	mem_free(client->cmd_r->shm);
malloc_shm_r_failed :
	mem_free(client->cmd_r->sem);
malloc_sem_r_failed :
	mem_free(client->cmd_r);
malloc_cmd_r_failed :
	mem_free(client);
malloc_client_failed :
	return NULL;
}
static void pc_req_destroy_client_req_cb(void *buf, void *pdata)
{
	struct pc_c_client *client = pdata;
	UINT8 client_id[MAX_CLIENT_ID_LEN] = {0};
	UINT8 *p = buf;
	/* | cmd | id_len | id | */
	UINT32_TO_STREAM(p, CMD_CLOSE_SERVER_REQ);
	UINT32_TO_STREAM(p, client->id->len);
	memcpy(client_id, client->id->data, client->id->len);
	ARRAY_TO_STREAM(p, client_id, MAX_CLIENT_ID_LEN);
}
static void pc_req_destroy_client_rsp_cb(void *buf, void *pdata)
{
	/* | cmd | state | */
	int cmd, state;
	UINT8 *p = buf;
	STREAM_TO_UINT32(cmd, p);
	STREAM_TO_UINT32(state, p);
	if(cmd != CMD_CLOSE_SERVER_RSP || state != CMD_SUCCESS){
		Log.e("req_destroy_client");
		return;
	}
}
int pc_req_destroy_client(struct pc_c_client* client)
{
	if(!client || !client->cmd_l || !client->cmd_r){
		return -1;
	}
	ipc_send_cmd(client->cmd_r,
			pc_req_destroy_client_req_cb,
			pc_req_destroy_client_rsp_cb, client);
	ipc_cmd_remote_detach(client->cmd_r);
	mem_free(client->cmd_l->sem);
	mem_free(client->cmd_l->shm);
	mem_free(client->cmd_r->sem);
	mem_free(client->cmd_r->shm);
	mem_free(client->cmd_l);
	mem_free(client->cmd_r);
	mem_free(client->id);
	mem_free(client);
	return 0;
}
static void pc_client_recv_cb(void *buf, void *pdata)
{
	//struct pc_c_client* client = pdata;
	//UINT8 *p = buf;
}
int pc_client_run(struct pc_c_client* client)
{
	int res;
	if(!client || !client->cmd_l){
		return -1;
	}
	while(1){
		res = ipc_recv_cmd(client->cmd_l, pc_client_recv_cb, NULL);
		if(res < 0){
			return res;
		}
	}
}
static int list_add_client(struct queue *list, struct pc_s_client *client)
{
	if(queue_push_tail(list, client) == false){
		return -1;
	}
	return 0;
}

struct id_data {
	void *id;
	int len;
};
bool list_find_client_cb(const void *data, const void *match_data)
{
	const struct pc_s_client *client = data;
	const struct id_data *id_data = match_data;
	if(client->id->len != id_data->len){
		return false;
	}
	if(!memcmp(client->id->data, id_data->id, id_data->len)){
		return true;
	}
	return false;
}
static struct pc_s_client* list_find_client(
		struct queue *list, void *id, int id_len)
{
	struct pc_s_client *client;
	struct id_data id_data;
	id_data.id = id;
	id_data.len = id_len;
	client = queue_find(list, list_find_client_cb, &id_data);
	return client;
}
static int list_remove_client(struct queue *list, struct pc_s_client *client)
{
	if(queue_remove(list, client) == false){
		return -1;
	}
	return 0;
}
