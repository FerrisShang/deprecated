#include "ipc_shm.h"

#include <unistd.h>
#include <stdlib.h>

#define TEXT_SZ 2048


#define SEMPERM 0666

struct ipc_shm* ipc_create_shm(key_t key, size_t size)
{
	struct ipc_shm *shm = NULL;
	void *buf;
	int shmid;
	shm = mem_malloc(sizeof(struct ipc_shm));
	if(!shm){
		goto malloc_failed;
	}
	shmid = shmget(key, size, SEMPERM | IPC_CREAT | IPC_EXCL);
	if(shmid < 0){
		goto shmget_failed;
	}
	buf = shmat(shmid, 0, 0);
	if(buf < 0){
		goto shmat_failed;
	}
	shm->key = key;
	shm->size = size;
	shm->shmid = shmid;
	shm->buf = buf;
	return shm;
shmat_failed :
	shmctl(shmid, IPC_RMID, 0);
shmget_failed :
	mem_free(shm);
malloc_failed :
	return NULL;
}
int ipc_find_shm_by_id(struct ipc_shm *shm)
{
	if(!shm || shm->shmid <= 0){
	   return -1;
	}
	shm->buf = shmat(shm->shmid, 0, 0);
	if(shm->buf < 0){
		return -1;
	}
	return 0;
}
int ipc_find_shm_by_key(struct ipc_shm *shm)
{
	if(!shm){
		return -1;
	}
	shm->shmid = shmget(shm->key, shm->size, SEMPERM|IPC_CREAT|IPC_EXCL);
	if(shm->shmid < 0){
		shm->shmid = shmget(shm->key, shm->size, SEMPERM|IPC_CREAT);
		if(shm->shmid < 0){
			goto find_failed;
		}
		shm->buf = shmat(shm->shmid, 0, 0);
		if(shm->buf < 0){
			goto find_failed;
		}
		return 0;
	}else{
		//share memory not exsit
		shmctl(shm->shmid, IPC_RMID, 0);
		goto find_failed;
	}
find_failed :
	return -1;
}
static int ipc_free_shm(int shmid, void *shm)
{
	if(shmdt(shm) == -1){
		return -1;
	}
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		return -1;
	}
	return 0;
}
int ipc_destroy_shm(struct ipc_shm *ipc_shm)
{
	int res;
	res = ipc_free_shm(ipc_shm->shmid, ipc_shm->buf);
	if(res < 0){
		return res;
	}
	mem_free(ipc_shm);
	return 0;
}
