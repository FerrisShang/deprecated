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

/******************************** ipc_cmd test ********************************/

static int cnt;
void server_cb(void *buf, void *pdata)
{
	int *b = buf;
	int tmp = *b;
	//printf("0x%08x\n", *b);
	memcpy(b, b+512, 512);
	memcpy(b+512, b, 512);
	*b = ~tmp;
	//printf("%s@%d:%d\n", __func__, __LINE__, (unsigned char)b[0]);
}
void request_cb(void *buf, void *pdata)
{
	int *b = buf;
	*b = rand()&0x00FFFFFF;
	cnt = *b;
	//printf("%s@%d:%d\n", __func__, __LINE__, (unsigned char)b[0]);
	*b++ = 0;
	*b++ = rand();
	*b++ = rand();
	*b++ = rand();
	*b++ = rand();
	int i;
	for(i=0;i<16;i++){
		printf("%02x ", ((UINT8*)buf)[i]);
	}
	printf("  send\n");
}
void response_cb(void *buf, void *pdata)
{
	//int *b = buf;
	//if(*b != ~cnt){
	//	printf("error\n");
	//}
	//printf("%s@%d:%d\n", __func__, __LINE__, (unsigned char)b[0]);
	int i;
	for(i=0;i<16;i++){
		printf("%02x ", ((UINT8*)buf)[i]);
	}
	printf("  recv\n");
}

void* close_server(void *pdata)
{
	struct pc_server *server = pdata;
	usleep(10000000);
	pc_destroy_server(server);
	return NULL;
}
void out_cb(void *buf, void *pdata)
{
	UINT8 *p = buf;
	UINT32_TO_STREAM(p, 16);
	UINT32_TO_STREAM(p, rand());
	UINT32_TO_STREAM(p, rand());
	UINT32_TO_STREAM(p, rand());
	UINT32_TO_STREAM(p, rand());
}
void ret_cb(void *buf, void *pdata)
{
	Log.v("%s@%d, buf=%d", __func__, __LINE__, *(UINT8*)buf);
}
struct msg {
	struct pc_c_client *client;
	char *id;
	int len;
};
void* send_msg(void *pdata)
{
	struct msg *msg = pdata;
	usleep(5000000);
	Log.v("sending data");
	pc_c_send(msg->client,
			msg->id,
			msg->len,
			out_cb, ret_cb, NULL);
	return NULL;
}
int main(int argc, char *argv[])
{
	pthread_t server_t, client_t;
	struct pc_server *server;
	struct pc_c_client *client;
	struct msg msg;
	char client_id[17];
	srand((int)time(0));
	server = pc_create_server(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE);
	if(server){
		pthread_create(&server_t, NULL, close_server, server);
		pc_server_run(server);
		pthread_join(server_t, NULL);
		mem_dump();
		return 0;
	}
	Log.e("pc_create_server failed");

	strcpy(client_id, "1234567890123456");
	client = pc_req_create_client(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
		client_id, 16, NULL, NULL);
	if(!client){
		Log.e("pc_req_create_client failed");
		strcpy(client_id, "6543210987654321");
		client = pc_req_create_client(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE,
				client_id, 16, NULL, NULL);
		if(!client){
			Log.e("pc_req_create_client failed again..");
			return 0;
		}
	}
	msg.client = client;
	msg.id = client_id;
	msg.len = 16;
	pthread_create(&client_t, NULL, send_msg, &msg);
	pc_client_run(client);
	pthread_join(client_t, NULL);
	pc_req_destroy_client(client);
	mem_dump();
	return 0;
}
