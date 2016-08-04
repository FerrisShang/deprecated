#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"
#include "proc_comm.h"
#include "types.h"

/*
 * format:
 *     command | data
 * command | data:
 *
 */

enum{
	CMD_SUCCESS          =  0,
	CMD_FAILED           = -1,
	CMD_CLIENT_NOT_FOUNT = -2,
};
enum{
	CMD_GET_SERVER_REQ = 0,
	CMD_GET_SERVER_RSP,
	CMD_CLOSE_SERVER_REQ,
	CMD_CLOSE_SERVER_RSP,
	CMD_GET_CLIENT_REQ,
	CMD_GET_CLIENT_RSP,
	CMD_DATA_SEND_REQ,
	CMD_DATA_SEND_RSP,
};
#define RESERVE_SIZE (64) //lager than header of request & recevie
#define CMD_DATA_SEND_REQ_RESERVICE_SIZE 16

struct pc_server {
	struct ipc_cmd *cmd;
	pc_is_broadcast_cb is_broadcase_cb;
	pc_is_bc_match_cb is_bc_match_cb;
	//client data
	struct queue *client_list; /* struct pc_s_client */
};
struct client_id {
	char data[MAX_CLIENT_ID_LEN];
	int len;
};
struct pc_c_client {
	struct ipc_cmd *cmd_l;
	struct ipc_cmd *cmd_server;
	struct client_id *id;
	struct queue *dev_list; /* struct pc_s_client */
	pc_is_broadcast_cb is_broadcase_cb;
	pc_recv_cb recv_cb;
	void *pdata;
	pid_t pid;
};
struct pc_s_client {
	struct ipc_cmd *client_cmd;
	struct client_id *id;
	pid_t pid;
};

struct pc_comm_data {
	UINT32 *id_len;
	void *s_id;
	void *d_id;
	UINT32 *data_len;
	UINT8 *data;
	int err;
};

static struct pc_s_client* create_remote_server(
		struct pc_server* server, int buffer_size, void *id, int id_len, pid_t pid);
static int destroy_remote_server(struct pc_server* server ,void *id, int id_len);
//static int pc_destroy_client(struct pc_server* server, struct pc_s_client* client);
static int pc_s_send(struct pc_s_client* client, struct pc_comm_data *data_send);
static void broadcast_send(struct pc_server *server, struct pc_comm_data *data_send);
static struct pc_s_client* find_client(struct queue *list, void *id, int id_len);
static int add_client(struct pc_server *server, struct pc_s_client *client);
static int remove_client(struct pc_server *server, struct pc_s_client *client);

struct pc_server* pc_create_server(key_t sem_key, key_t shm_key, int buffer_size,
		pc_is_broadcast_cb is_broadcase_cb,
		pc_is_bc_match_cb is_bc_match_cb)
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
	server->cmd = ipc_create_cmd(sem_key, shm_key, RESERVE_SIZE + buffer_size);
	if(!server->cmd){
		goto ipc_create_cmd_failed;
	}
	server->is_broadcase_cb = is_broadcase_cb;
	server->is_bc_match_cb = is_bc_match_cb;
	return server;
ipc_create_cmd_failed :
	queue_destroy(server->client_list, NULL);
queue_new_failed :
	mem_free(server);
malloc_server_failed :
	return NULL;
}

static struct pc_s_client* create_remote_server(
		struct pc_server* server, int buffer_size, void *id, int id_len, pid_t pid)
{
	struct pc_s_client* client;
	client = find_client(server->client_list, id, id_len);
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
	client->pid = pid;
	client->client_cmd = ipc_create_cmd(IPC_PRIVATE, IPC_PRIVATE,
			RESERVE_SIZE + buffer_size);
	if(!client->client_cmd){
		Log.e("create client failed");
		goto ipc_create_cmd_failed;
	}
	if(add_client(server, client) < 0){
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
	client = find_client(server->client_list, id, id_len);
	if(!client){
		return -1;
	}
	remove_client(server, client);
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
		case CMD_GET_SERVER_REQ :{// | cmd | id_len | id | mem_size | pid |
			struct pc_s_client *pc_s_client;
			void *id;
			UINT32 mem_size, id_len, pid;
			STREAM_TO_UINT32(id_len, p);
			id = p;
			p += MAX_CLIENT_ID_LEN;
			STREAM_TO_UINT32(mem_size, p);
			STREAM_TO_UINT32(pid, p);
			Log.v("client request communicate channel");
			Log.v("\tID:0x%s", hex2str(id, id_len));
			Log.v("\tPID:%d", pid);
			Log.v("\tsize:%d", mem_size);
			pc_s_client = create_remote_server(server, mem_size, id, id_len, pid);
			if(!pc_s_client){
				Log.v("client request communicate channel failed");
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
			Log.v("client request communicate channel success");
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
		case CMD_GET_CLIENT_REQ :{// | cmd | id_len | id |
			struct pc_s_client *client;
			void *id;
			UINT32 id_len;
			STREAM_TO_UINT32(id_len, p);
			id = p;
			p += MAX_CLIENT_ID_LEN;
			client = find_client(server->client_list, id, id_len);
			if(!client){
				//client id not exsit
				p = buf;
				UINT32_TO_STREAM(p, CMD_GET_CLIENT_RSP);
				UINT32_TO_STREAM(p, CMD_FAILED);
				return;
			}
			/* | cmd | state | semid | size | shmid | size | */
			p = buf;
			UINT32_TO_STREAM(p, CMD_GET_CLIENT_RSP);
			UINT32_TO_STREAM(p, CMD_SUCCESS);
			UINT32_TO_STREAM(p, client->client_cmd->sem->semid);
			UINT32_TO_STREAM(p, client->client_cmd->sem->nsems);
			UINT32_TO_STREAM(p, client->client_cmd->shm->shmid);
			UINT32_TO_STREAM(p, client->client_cmd->shm->size);
			}break;
		case CMD_DATA_SEND_REQ :{
			/* | cmd | id_len | s_id | d_id | reserve | d_len | data | */
			struct pc_s_client *client;
			struct pc_comm_data data_send;
			data_send.id_len = (UINT32*)p;
			p += sizeof(UINT32);
			data_send.s_id = p;
			p += MAX_CLIENT_ID_LEN;
			data_send.d_id = p;
			p += MAX_CLIENT_ID_LEN;
			p += CMD_DATA_SEND_REQ_RESERVICE_SIZE;
			data_send.data_len = (UINT32*)p;
			p += sizeof(UINT32);
			data_send.data = p;

			if(server->is_broadcase_cb &&
					server->is_broadcase_cb(data_send.d_id, *data_send.id_len)){
				//broadcast
				broadcast_send(server, &data_send);
				p = buf;
				UINT32_TO_STREAM(p, CMD_DATA_SEND_RSP);
				UINT32_TO_STREAM(p, CMD_SUCCESS);
				break;
			}else{
				//p2p
				client = find_client(server->client_list,
						data_send.d_id, *data_send.id_len);
				if(!client){
					p = buf;
					UINT32_TO_STREAM(p, CMD_DATA_SEND_RSP);
					UINT32_TO_STREAM(p, CMD_CLIENT_NOT_FOUNT);
					return;
				}else{
					pc_s_send(client, &data_send);
					p = buf;
					UINT32_TO_STREAM(p, CMD_DATA_SEND_RSP);
					UINT32_TO_STREAM(p, data_send.err);
					return;
				}
			}
			}break;
		default :
			break;
	}
	return;
}
static void signal_handler_fun(int signal_num)
{
	return;
}
int pc_server_run(struct pc_server *server)
{
	signal(SIGINT, signal_handler_fun);
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

static void client_list_destroy_cb(void *data)
{
	struct pc_s_client *client = data;
	ipc_destroy_local_cmd(client->client_cmd);
	mem_free(client->id);
	mem_free(client);
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

/*
static int pc_destroy_client(struct pc_server* server, struct pc_s_client* client)
{
	return 0;
}
*/

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
	UINT32_TO_STREAM(p, client->cmd_server->shm->size);
	UINT32_TO_STREAM(p, client->pid);
}
static void pc_req_create_client_rsp_cb(void *buf, void *pdata)
{
	/* | cmd | state | semid | size | shmid | size | */
	struct pc_c_client *client = pdata;
	int cmd, state;

	UINT8 *p = buf;
	STREAM_TO_UINT32(cmd, p);
	STREAM_TO_UINT32(state, p);
	if(cmd != CMD_GET_SERVER_RSP || state != CMD_SUCCESS){
		Log.e("%s@%d:request client failed", __func__, __LINE__);
		return;
	}
	STREAM_TO_UINT32(client->cmd_l->sem->semid, p);
	STREAM_TO_UINT32(client->cmd_l->sem->nsems, p);
	STREAM_TO_UINT32(client->cmd_l->shm->shmid, p);
	STREAM_TO_UINT32(client->cmd_l->shm->size, p);
}
static struct ipc_cmd* create_empty_ipc_cmd(void)
{
	struct ipc_cmd *cmd;
	cmd = mem_malloc(sizeof(struct ipc_cmd));
	if(!cmd){
		goto malloc_cmd_failed;
	}
	cmd->sem = mem_malloc(sizeof(struct ipc_sem));
	if(!cmd->sem){
		goto malloc_sem_failed;
	}
	cmd->shm = mem_malloc(sizeof(struct ipc_shm));
	if(!cmd->shm){
		goto malloc_shm_failed;
	}
	return cmd;
malloc_shm_failed :
	mem_free(cmd->sem);
malloc_sem_failed :
	mem_free(cmd);
malloc_cmd_failed :
	return NULL;
}
static void destroy_empty_ipc_cmd(struct ipc_cmd* ipc_cmd)
{
	if(ipc_cmd){
		if(ipc_cmd->sem){
			mem_free(ipc_cmd->sem);
		}
		if(ipc_cmd->shm){
			mem_free(ipc_cmd->shm);
		}
		mem_free(ipc_cmd);
	}
}
struct pc_c_client* pc_req_create_client(key_t sem_key, key_t shm_key,
		int buffer_size,
		void *client_id, int id_len,
		pc_is_broadcast_cb is_broadcase_cb,
		pc_recv_cb callback, void *pdata)
{
	int res;
	struct pc_c_client *client;
	if(id_len == 0 || id_len > MAX_CLIENT_ID_LEN){
		Log.e("Invailed id length");
		return NULL;
	}
	if(!callback){
		Log.e("Invailed client callback");
		return NULL;
	}
	client = mem_malloc(sizeof(struct pc_c_client));
	if(!client){
		goto malloc_client_failed;
	}
	client->dev_list = queue_new();
	if(!client->dev_list){
		goto queue_new_failed;
	}
	client->cmd_server = create_empty_ipc_cmd();
	if(!client->cmd_server){
		goto malloc_cmd_r_failed;
	}
	client->cmd_l = create_empty_ipc_cmd();
	if(!client->cmd_l){
		goto malloc_cmd_l_failed;
	}
	client->id = mem_malloc(sizeof(struct client_id));
	if(!client->id){
		goto malloc_id_failed;
	}
	//init remote server
	client->pdata = pdata;
	client->cmd_server->sem->key = sem_key;
	client->cmd_server->shm->key = shm_key;
	client->cmd_server->shm->size = RESERVE_SIZE + buffer_size;
	res = ipc_cmd_remote_init(client->cmd_server);
	if(res < 0){
		goto ipc_cmd_r_init_failed;
	}
	//request local server
	memset(client->cmd_l->sem, 0, sizeof(struct ipc_sem));
	memset(client->cmd_l->shm, 0, sizeof(struct ipc_shm));
	memcpy(client->id->data, client_id, id_len);
	client->id->len = id_len;
	client->is_broadcase_cb = is_broadcase_cb;
	client->recv_cb = callback;
	client->pdata = pdata;
	client->pid = getpid();
	res = ipc_send_cmd(client->cmd_server,
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
	destroy_empty_ipc_cmd(client->cmd_l);
malloc_cmd_l_failed :
	destroy_empty_ipc_cmd(client->cmd_server);
malloc_cmd_r_failed :
	queue_destroy(client->dev_list, NULL);
queue_new_failed :
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

static void client_list_free_cb(void *data)
{
	struct pc_s_client *r_dev = data;
	ipc_cmd_remote_detach(r_dev->client_cmd);
	destroy_empty_ipc_cmd(r_dev->client_cmd);
	mem_free(r_dev->id);
	mem_free(r_dev);
}
int pc_req_destroy_client(struct pc_c_client* client)
{
	if(!client || !client->cmd_l || !client->cmd_server){
		return -1;
	}
	ipc_send_cmd(client->cmd_server,
			pc_req_destroy_client_req_cb,
			pc_req_destroy_client_rsp_cb, client);
	ipc_cmd_remote_detach(client->cmd_server);
	destroy_empty_ipc_cmd(client->cmd_l);
	destroy_empty_ipc_cmd(client->cmd_server);
	mem_free(client->id);
	queue_destroy(client->dev_list, client_list_free_cb);
	mem_free(client);
	return 0;
}
static void pc_client_recv_cb(void *buf, void *pdata)
{
	struct pc_c_client* client = pdata;
	struct pc_comm_data data_recv;
	UINT32 cmd;
	UINT8 *p = buf;
	/* | cmd | id_len | s_id | d_id | reserve | d_len | data | */

	STREAM_TO_UINT32(cmd, p);
	switch(cmd){
		case CMD_DATA_SEND_REQ :
			data_recv.id_len = (UINT32*)p;
			p += sizeof(UINT32);
			data_recv.s_id = p;
			p += MAX_CLIENT_ID_LEN;
			data_recv.d_id = p;
			p += MAX_CLIENT_ID_LEN;
			p += CMD_DATA_SEND_REQ_RESERVICE_SIZE;
			data_recv.data_len = (UINT32*)p;
			p += sizeof(UINT32);
			data_recv.data = p;
			client->recv_cb(data_recv.s_id, *data_recv.id_len,
					data_recv.data, *data_recv.data_len, client->pdata);
			p = buf;
			UINT32_TO_STREAM(p, CMD_DATA_SEND_RSP);
			UINT32_TO_STREAM(p, CMD_SUCCESS);
			break;
		default :
			Log.v("%s@%d : receive data error", __func__, __LINE__);
			p = buf;
			UINT32_TO_STREAM(p, CMD_DATA_SEND_RSP);
			UINT32_TO_STREAM(p, CMD_FAILED);
			break;
	}
}
int pc_client_run(struct pc_c_client* client)
{
	int res;
	if(!client || !client->cmd_l){
		return -1;
	}
	while(1){
		res = ipc_recv_cmd(client->cmd_l, pc_client_recv_cb, client);
		if(res < 0){
			return res;
		}
	}
}
static int add_client(struct pc_server *server, struct pc_s_client *client)
{
	struct queue *list = server->client_list;
	if(queue_push_tail(list, client) == false){
		return -1;
	}
	return 0;
}

struct find_client_data {
	struct pc_server *server;
	void *id;
	int len;
};
static bool list_find_client_cb(const void *data, const void *match_data)
{
	const struct pc_s_client *client = data;
	const struct find_client_data *d = match_data;
	if(client->id->len != d->len){
		return false;
	}
	if(!memcmp(client->id->data, d->id, d->len)){
		return true;
	}
	return false;
}
static struct pc_s_client* find_client(
		struct queue *list, void *id, int id_len)
{
	struct pc_s_client *client;
	struct find_client_data d;
	d.id = id;
	d.len = id_len;
	client = queue_find(list, list_find_client_cb, &d);
	return client;
}
static int remove_client(struct pc_server *server, struct pc_s_client *client)
{
	struct queue *list = server->client_list;
	if(queue_remove(list, client) == false){
		return -1;
	}
	return 0;
}


static void send_out_cb(void *buf, void *pdata)
{
	struct pc_comm_data *data_send = pdata;
	UINT8 *p = buf;
	UINT8 id[MAX_CLIENT_ID_LEN] = {0};
	/* | cmd | id_len | s_id | d_id | reserve | d_len | data | */
	UINT32_TO_STREAM(p, CMD_DATA_SEND_REQ);
	UINT32_TO_STREAM(p, *data_send->id_len);
	memcpy(id, data_send->s_id, *data_send->id_len);
	ARRAY_TO_STREAM(p, id, MAX_CLIENT_ID_LEN);
	memcpy(id, data_send->d_id, *data_send->id_len);
	ARRAY_TO_STREAM(p, id, MAX_CLIENT_ID_LEN);
	p += CMD_DATA_SEND_REQ_RESERVICE_SIZE;
	UINT32_TO_STREAM(p, *data_send->data_len);
	ARRAY_TO_STREAM(p, data_send->data, *(data_send->data_len));
}
static void send_ret_cb(void *buf, void *pdata)
{
	struct pc_comm_data *data_send = pdata;
	UINT8 *p = buf;
	int cmd, state;
	/* | cmd | state | */
	STREAM_TO_UINT32(cmd, p);
	STREAM_TO_UINT32(state, p);
	if(cmd != CMD_DATA_SEND_RSP){
		data_send->err = CMD_FAILED;
		return;
	}
	data_send->err = state;
}
static int pc_s_send(struct pc_s_client* client, struct pc_comm_data *data_send)
{
	int res;
	res = ipc_send_cmd(client->client_cmd, send_out_cb, send_ret_cb, data_send);
	if(res < 0){
		data_send->err = CMD_FAILED;
	}
	return res;
}

struct broadcast_data {
	struct pc_server *server;
	struct pc_s_client *client;
	struct pc_comm_data *data;
};

static void broadcast_send_cb(void *data, void *user_data)
{
	struct broadcast_data *bc_data = user_data;
	struct pc_server *server = bc_data->server;
	struct pc_comm_data *d = bc_data->data;
	struct pc_s_client *client = data;
	if(client != bc_data->client){
		if(!server->is_bc_match_cb ||
				server->is_bc_match_cb(d->d_id, client->id->data, *d->id_len)){
			pc_s_send(client, bc_data->data);
		}
	}else{
	}
}
static void broadcast_send(struct pc_server *server, struct pc_comm_data *data_send)
{
	struct pc_s_client *s_client;
	struct broadcast_data bc_data;
	s_client = find_client(server->client_list, data_send->s_id, *data_send->id_len);
	bc_data.server = server;
	bc_data.client = s_client;
	bc_data.data = data_send;
	queue_foreach(server->client_list, broadcast_send_cb, &bc_data);
}
struct pc_c_send_data {
	struct pc_c_client* client;
	char *id;
	int id_len;
	pc_send_out_cb out_cb;
	pc_send_return_cb ret_cb;
	void *pdata;
	int err;
};
static void pc_c_send_out_cb(void *buf, void *pdata)
{
	struct pc_c_send_data *data = pdata;
	UINT8 *p = buf;
	int *data_len;
	UINT8 id[MAX_CLIENT_ID_LEN] = {0};
	/* | cmd | id_len | s_id | d_id | reserve | d_len | data | */
	UINT32_TO_STREAM(p, CMD_DATA_SEND_REQ);
	UINT32_TO_STREAM(p, data->client->id->len);
	memcpy(id, data->client->id->data, data->client->id->len);
	ARRAY_TO_STREAM(p, id, MAX_CLIENT_ID_LEN);
	memcpy(id, data->id, data->id_len);
	ARRAY_TO_STREAM(p, id, MAX_CLIENT_ID_LEN);
	p += CMD_DATA_SEND_REQ_RESERVICE_SIZE;
	data_len = (int*)p;
	p += sizeof(UINT32);
	data->out_cb(p, data_len, data->pdata);
}
static void pc_c_send_ret_cb(void *buf, void *pdata)
{
	struct pc_c_send_data *data = pdata;
	UINT8 *p = buf;
	UINT32 cmd;
	UINT32 status;
	/* | cmd | state | */
	STREAM_TO_UINT32(cmd, p);
	STREAM_TO_UINT32(status, p);
	if(cmd != CMD_DATA_SEND_RSP){
		Log.e("%s@%d : send data response error", __func__, __LINE__);
		return;
	}
	data->ret_cb(status, data->pdata);
}
static void pc_c_get_client_req_cb(void *buf, void *pdata)
{
	struct pc_c_send_data *data = pdata;
	UINT8 *p = buf;
	UINT8 id[MAX_CLIENT_ID_LEN] = {0};
	/* | cmd | id_len | id | */
	UINT32_TO_STREAM(p, CMD_GET_CLIENT_REQ);
	UINT32_TO_STREAM(p, data->id_len);
	memcpy(id, data->id, data->id_len);
	ARRAY_TO_STREAM(p, id, MAX_CLIENT_ID_LEN);
}
static void pc_c_get_client_rsp_cb(void *buf, void *pdata)
{
	struct pc_c_send_data *data = pdata;
	struct pc_c_client *client = data->client;
	struct pc_s_client *dev;
	int cmd, res;

	/* | cmd | state | semid | size | shmid | size | */
	UINT8 *p = buf;
	STREAM_TO_UINT32(cmd, p);
	STREAM_TO_UINT32(data->err, p);
	if(cmd != CMD_GET_CLIENT_RSP || data->err != CMD_SUCCESS){
		Log.e("%s@%d:request client failed", __func__, __LINE__);
		return;
	}

	dev = mem_malloc(sizeof(struct pc_s_client));
	if(!dev){
		goto malloc_dev_failed;
	}
	dev->client_cmd = create_empty_ipc_cmd();
	if(!dev->client_cmd){
		goto create_ipc_cmd_failed;
	}
	dev->id = mem_malloc(sizeof(struct client_id));
	if(!dev->id){
		goto malloc_id_failed;
	}
	STREAM_TO_UINT32(dev->client_cmd->sem->semid, p);
	STREAM_TO_UINT32(dev->client_cmd->sem->nsems, p);
	STREAM_TO_UINT32(dev->client_cmd->shm->shmid, p);
	STREAM_TO_UINT32(dev->client_cmd->shm->size, p);
	dev->client_cmd->sem->key = 0;
	dev->client_cmd->shm->key = 0;
	res = ipc_cmd_remote_init(dev->client_cmd);
	if(res < 0){
		goto remote_init_failed;
	}
	dev->id->len = data->id_len;
	memcpy(dev->id->data, data->id, dev->id->len);
	if(true != queue_push_head(client->dev_list, dev)){
		goto remote_init_failed;
	}
	data->err = CMD_SUCCESS;
	return;
remote_init_failed :
	mem_free(dev->id);
malloc_id_failed :
	destroy_empty_ipc_cmd(dev->client_cmd);
create_ipc_cmd_failed :
	mem_free(dev);
malloc_dev_failed :
	data->err = CMD_FAILED;
}
int pc_c_send(struct pc_c_client* client, char *id, int id_len,
		pc_send_out_cb out_cb, pc_send_return_cb ret_cb, void *pdata)
{
	struct pc_c_send_data data;
	if(!client){
		return -1;
	}
	if(id_len == 0 || id_len > MAX_CLIENT_ID_LEN){
		Log.e("Invailed id length");
		return -1;
	}
	if(!out_cb || !ret_cb){
		Log.e("Invailed client callback");
		return -1;
	}
	data.client = client;
	data.id = id;
	data.id_len = id_len;
	data.out_cb = out_cb;
	data.ret_cb = ret_cb;
	data.pdata = pdata;
	if(client->is_broadcase_cb &&
			client->is_broadcase_cb(id, id_len)){
		//broadcast
		return ipc_send_cmd(client->cmd_server, pc_c_send_out_cb,
				pc_c_send_ret_cb, &data);
	}else{
		//p2p
		int res;
		struct pc_s_client *r_dev;
		r_dev = find_client(client->dev_list, id, id_len);
		if(!r_dev){
			res = ipc_send_cmd(client->cmd_server, pc_c_get_client_req_cb,
					pc_c_get_client_rsp_cb, &data);
			if(res < 0){
				return res;
			}
			r_dev = find_client(client->dev_list, id, id_len);
			if(!r_dev){
				Log.d("Destination client(%s) not exsit", hex2str(id,id_len));
				return CMD_FAILED;
			}
		}
		res = ipc_send_cmd(r_dev->client_cmd, pc_c_send_out_cb,
				pc_c_send_ret_cb, &data);
		if(res < 0){
			//send failed, maybe remote closed, remove device from list
			queue_remove(client->dev_list, r_dev);
			client_list_free_cb(r_dev);
			return CMD_FAILED;
		}
		return data.err;
	}
}
