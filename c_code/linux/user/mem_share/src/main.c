#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "unistd.h"
#include "wi_bus.h"
#include "ipc_cmd.h"

#define IPC_SHM_KEY  0xdead
#define IPC_SEM_KEY 0xdead
#define SHMGET_SIZE 4000

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
}
void response_cb(void *buf, void *pdata)
{
	int *b = buf;
	if(*b != ~cnt){
		printf("error\n");
	}
	//printf("%s@%d:%d\n", __func__, __LINE__, (unsigned char)b[0]);
}
int main(int argc, char *argv[])
{
#if 0
	struct ipc_cmd_local *ipc_cmd;
	int i;
	ipc_cmd = ipc_create_cmd(IPC_SEM_KEY, IPC_SHM_KEY, SHMGET_SIZE);
	if(!ipc_cmd){
		printf("create cmd error\n");
		return -1;
	}
	printf("%p,%p,%p\n", ipc_cmd, ipc_cmd->shm, ipc_cmd->sem);

	for(i=0;i<1000000;i++){
		ipc_recv_cmd(ipc_cmd, server_cb, NULL);
	}
	ipc_destroy_local_cmd(ipc_cmd);
	mem_dump();
#else
	struct ipc_cmd_remote *ipc_cmd;
	ipc_cmd = mem_malloc(sizeof(struct ipc_cmd_remote));
	ipc_cmd->shm = mem_malloc(sizeof(struct ipc_shm));
	ipc_cmd->sem = mem_malloc(sizeof(struct ipc_sem));
	ipc_cmd->shm->key = IPC_SHM_KEY;
	ipc_cmd->shm->size = SHMGET_SIZE;
	ipc_cmd->sem->key = IPC_SEM_KEY;
	int res = ipc_cmd_remote_init(ipc_cmd);
	if(res < 0){
		printf("init cmd error\n");
		return -1;
	}
	/*
	ipc_cmd_remote_detach(ipc_cmd);
	res = ipc_cmd_remote_init(ipc_cmd);
	if(res < 0){
		printf("init cmd error second\n");
		return -1;
	}
	*/
	printf("%p,%p,%p\n", ipc_cmd, ipc_cmd->shm, ipc_cmd->sem);
	while(1){
		res = ipc_send_cmd(ipc_cmd, request_cb, response_cb, NULL);
		if(res < 0){
			break;
		}
	}
	ipc_cmd_remote_detach(ipc_cmd);
	mem_dump();
#endif
	return 0;
}
