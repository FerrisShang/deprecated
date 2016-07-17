#include <stdio.h>
#include <string.h>
#include "proc_comm.h"

/*
 * format:
 *     command | data
 * command | data:
 *     req: CMD_GET_SERVER_REQ | null
 *     rsp: CMD_GET_SERVER_RSP | result
 *
 *
 *
 */
enum{
	CMD_SUCCESS,
	CMD_FAILED,
};
enum{
	CMD_GET_SERVER_REQ,
	CMD_GET_SERVER_RSP,
	CMD_CLOSE_SERVER_REQ,
	CMD_CLOSE_SERVER_RSP,
	CMD_DATA_SEND_REQ,
	CMD_DATA_SEND_RSP,
};
#define RESERVE_SIZE 32

static struct pc_s_client* pc_create_remote_server(key_t sem_key, key_t shm_key,
		int buffer_size, void *client_data, int data_len, void *pdata);
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
	server->pdata = pdata;
	server->recv_cb = recv_cb;
	server->conn_state_change_req = conn_state_change_req;
	server->cmd = ipc_create_cmd(sem_key, shm_key, RESERVE_SIZE + buffer_size);
	if(!server->cmd){
		goto ipc_create_cmd_failed;
	}
	return server;
ipc_create_cmd_failed :
	mem_free(server);
malloc_server_failed :
	return NULL;
}

static struct pc_s_client* pc_create_remote_server(key_t sem_key, key_t shm_key,
		int buffer_size, void *client_data, int data_len, void *pdata)
{
	struct pc_s_client* client;
	if(data_len > MAX_CLIENT_DATA_LEN){
		return NULL;
	}
	client = mem_malloc(sizeof(struct pc_s_client));
	if(!client){
		goto malloc_client_failed;
	}
	client->c_data = mem_malloc(sizeof(struct client_data));
	if(!client->c_data){
		goto malloc_c_data_failed;
	}
	memcpy(client->c_data->data, client_data, data_len);
	client->c_data->len = data_len;
	client->pdata = pdata;
	client->cmd_r = NULL;
	client->cmd_l = ipc_create_cmd(sem_key, shm_key, RESERVE_SIZE + buffer_size);
	if(!client->cmd_l){
		goto ipc_create_cmd_failed;
	}
	return client;
ipc_create_cmd_failed :
	mem_free(client->c_data);
malloc_c_data_failed :
	mem_free(client);
malloc_client_failed :
	return NULL;
}
static void pc_server_recv_cb(void *buf, void *pdata)
{
	struct pc_server* server = pdata;
	char *b = buf;
	//server->pdata
	switch(b[0]){
		case CMD_GET_SERVER_REQ :
			break;
		case CMD_CLOSE_SERVER_REQ:
			break;
		default :
			break;
	}
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
int pc_destroy_server(struct pc_server* server)
{
	if(!server){
		return -1;
	}
	if(server->cmd){
		ipc_destroy_local_cmd(server->cmd);
	}
	mem_free(server);
	return 0;
}

int pc_destroy_client(struct pc_s_client* client)
{
	/*
	if(!client){
		return -1;
	}
	if(client->c_data){
		mem_free(client->c_data);
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
}
static void pc_req_create_client_rsp_cb(void *buf, void *pdata)
{
}
struct pc_c_client* pc_req_create_client(key_t sem_key, key_t shm_key,
		int buffer_size,
		void *client_data, int data_len,
		pc_recv_cb callback, void *pdata)
{
	int res;
	struct pc_c_client *client;
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
	res = ipc_send_cmd(client->cmd_r,
			pc_req_create_client_req_cb, pc_req_create_client_rsp_cb, NULL);
	if(res < 0){
		goto request_local_server_failed;
	}
	return client;
request_local_server_failed :
ipc_cmd_r_init_failed :
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

static void pc_client_recv_cb(void *buf, void *pdata)
{
	struct pc_c_client* client = pdata;
	char *b = buf;
}
int pc_client_run(struct pc_c_client* client)
{
	int res;
	if(!client || !client->cmd_l){
		return -1;
	}
	while(1){
		//res = ipc_recv_cmd(client->cmd_l, pc_client_recv_cb, );
		if(res < 0){
			return res;
		}
	}
}
