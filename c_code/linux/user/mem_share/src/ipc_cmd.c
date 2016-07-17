#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "ipc_cmd.h"

#define CMD_NSEMS        3
#define SEM_NSEM_SEND    0
#define SEM_NSEM_C_READ  1
#define SEM_NSEM_S_READ  2
static const int cmd_sem_value[] = {1, 0, 0};

struct ipc_cmd_local* ipc_create_cmd(key_t sem_key, key_t shm_key, size_t size)
{
	struct ipc_cmd_local *cmd;
	struct ipc_sem* sem;
	struct ipc_shm* shm;
	cmd = mem_malloc(sizeof(struct ipc_cmd_local));
	if(!cmd){
		goto malloc_failed;
	}
	sem = ipc_create_sem(sem_key, CMD_NSEMS);
	if(!sem){
		goto create_sem_failed;
	}
	shm = ipc_create_shm(shm_key, size);
	if(!shm){
		goto create_shm_failed;
	}
	if(ipc_init_sem(sem, (int*)cmd_sem_value)<0){
		goto init_sem_failed;
	}
	cmd->sem = sem;
	cmd->shm = shm;
	return cmd;
init_sem_failed :
	ipc_destroy_shm(shm);
create_shm_failed :
	ipc_destroy_sem(sem);
create_sem_failed :
	mem_free(cmd);
malloc_failed :
	return NULL;
}

int ipc_cmd_remote_init(struct ipc_cmd_remote *ipc_cmd)
{
	int res;
	if(!ipc_cmd || !ipc_cmd->sem || !ipc_cmd->shm){
		return -1;
	}
	if(ipc_cmd->sem->key > 0 && ipc_cmd->shm->key > 0){//init by keys
		ipc_cmd->sem->nsems = CMD_NSEMS;
		res = ipc_find_sem(ipc_cmd->sem);
		if(res < 0){
			goto init_sem_failed;
		}
		res = ipc_find_shm_by_key(ipc_cmd->shm);
		if(res < 0){
			goto find_shm_failed;
		}
		return 0;
	}else if(ipc_cmd->sem->semid > 0 && ipc_cmd->sem->nsems > 0 &&
			ipc_cmd->shm->shmid > 0 && ipc_cmd->shm->size > 0){//init by IDs
		ipc_cmd->sem->nsems = CMD_NSEMS;
		res = ipc_find_shm_by_id(ipc_cmd->shm);
		if(res < 0){
			goto find_shm_failed;
		}
		return 0;
	}
find_shm_failed :
init_sem_failed :
	return -1;
}

int ipc_cmd_remote_detach(struct ipc_cmd_remote *ipc_cmd)
{
	ipc_detach_shm(ipc_cmd->shm);
	return 0;
}

int ipc_destroy_local_cmd(struct ipc_cmd_local *ipc_cmd)
{
	ipc_destroy_sem(ipc_cmd->sem);
	ipc_destroy_shm(ipc_cmd->shm);
	mem_free(ipc_cmd);
	return 0;
}
int ipc_send_cmd(struct ipc_cmd_remote *ipc_cmd,
		ipc_cmd_io_cb request_cb, ipc_cmd_io_cb response_cb, void *pdata)
{
	int res;
	res = ipc_sem_p(ipc_cmd->sem->semid, SEM_NSEM_SEND);
	if(res < 0){return res;}
	request_cb(ipc_cmd->shm->buf, pdata);
	res = ipc_sem_v(ipc_cmd->sem->semid, SEM_NSEM_S_READ);
	if(res < 0){return res;}
	res = ipc_sem_p(ipc_cmd->sem->semid, SEM_NSEM_C_READ);
	if(res < 0){return res;}
	if(response_cb){
		response_cb(ipc_cmd->shm->buf, pdata);
	}
	res = ipc_sem_v(ipc_cmd->sem->semid, SEM_NSEM_SEND);
	if(res < 0){return res;}
	return 0;
}
int ipc_recv_cmd(struct ipc_cmd_local *ipc_cmd, ipc_cmd_io_cb callback, void *pdata)
{
	int res;
	res = ipc_sem_p(ipc_cmd->sem->semid, SEM_NSEM_S_READ);
	if(res < 0){return res;}
	callback(ipc_cmd->shm->buf, pdata);
	res = ipc_sem_v(ipc_cmd->sem->semid, SEM_NSEM_C_READ);
	if(res < 0){return res;}
	return 0;
}
