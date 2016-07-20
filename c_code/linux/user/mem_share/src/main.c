#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "unistd.h"
#include "wi_bus.h"
#include "proc_comm.h"
#include "types.h"
#define IPC_SHM_KEY ((key_t)0X005FC0DE)
#define IPC_SEM_KEY ((key_t)0X005FC0DE)
#define SHMGET_SIZE 2000

struct msg {
	struct pc_c_client *client;
	char *id;
	int len;
};
void* close_server(void *pdata)
{
	struct pc_server *server = pdata;
	usleep(10000000);
	pc_destroy_server(server);
	return NULL;
}
void client_out_cb(void *buf, int *len, void *pdata)
{
	struct msg *msg = pdata;
	UINT8 *p = buf;
	*len = 16;
	UINT32_TO_STREAM(p, rand());
	UINT32_TO_STREAM(p, rand());
	UINT32_TO_STREAM(p, rand());
	UINT32_TO_STREAM(p, rand());
	Log.v("request send data:");
	Log.v("\tfrom:self to 0x%s",
			hex2str(msg->id, msg->len));
	Log.v("\tlength:%d", 16);
	Log.v("\tdata:%s", hex2str(buf, 32));
	Log.v("\t     %s", hex2str(buf+32, 32));

}
void client_ret_cb(int status, void *pdata)
{
	//Log.v("%s@%d, status=%d", __func__, __LINE__, status);
}
void client_recv_data(void *id, int id_len, void *buf, int buf_len, void *pdata)
{
	Log.v("receive data:");
	Log.v("\tfrom:0x%s to %s",
			hex2str(id, id_len),
			"self");
	Log.v("\tlength:%d", buf_len);
	Log.v("\tdata:%s", hex2str(buf, 32));
	Log.v("\t     %s", hex2str(buf+32, 32));
}
void* send_msg(void *pdata)
{
	struct msg *msg = pdata;
	int res;
	while(1){
		if(*(char*)msg->id == '0'){
			break;
		}
		res = pc_c_send(msg->client,
				msg->id,
				msg->len,
				client_out_cb, client_ret_cb, msg);
		if(res < 0){
			return NULL;
		}
		usleep(5000000);
	}
	return NULL;
}

bool is_broadcast_cb(void *id, int id_len);
bool is_bc_match_cb(void *id_bc, void *id, int id_len);
int main(int argc, char *argv[])
{
	struct pc_server *server;
	struct pc_c_client *client;
	struct msg msg;
	char client_id[17];
	char remote_id[17];
	int id_len;
	srand((int)time(0));
	if(argc == 1){
		server = pc_create_server(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
				is_broadcast_cb, is_bc_match_cb);
		if(server){
			//pthread_t server_t;
			//pthread_create(&server_t, NULL, close_server, server);
			pc_server_run(server);
			//pthread_join(server_t, NULL);
			mem_dump();
			return 0;
		}
		Log.e("pc_create_server failed");
	}else if(argc == 3){
		pthread_t client_t;
		strcpy(client_id, argv[1]);
		strcpy(remote_id, argv[2]);
		id_len = strlen(client_id);
		client = pc_req_create_client(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
				client_id, id_len, client_recv_data, NULL);
		if(!client){
			Log.e("pc_req_create_client failed");
			return 0;
		}
		msg.client = client;
		msg.id = remote_id;
		msg.len = id_len;
		pthread_create(&client_t, NULL, send_msg, &msg);
		pc_client_run(client);
		pthread_join(client_t, NULL);
		pc_req_destroy_client(client);
		mem_dump();
		return 0;
	}
	return 0;
}
bool is_broadcast_cb(void *id, int id_len)
{
	if(*(char*)id == 'f'){
		return true;
	}else{
		return false;
	}
}
bool is_bc_match_cb(void *id_bc, void *id, int id_len)
{
#if 1
	if(*(char*)id == '1'){
		return true;
	}else{
		return false;
	}
#else
	return true;
#endif
}
