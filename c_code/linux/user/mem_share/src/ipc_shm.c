#include "ipc_shm.h"
#define TEXT_SZ 2048

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue.h>

#define SEMPERM 0666

struct memory {
	int shmid;
	void *shm;
};
static struct queue *memory_list;
static int add_memory_to_list(int shmid, void *shm);
static int free_shared_memory(int shmid, void *shm);

void* create_ipc_shm(key_t key, size_t size)
{
	void *shm = NULL;
	int shmid;
	shmid = shmget(key, size, SEMPERM | IPC_CREAT | IPC_EXCL);
	if(shmid < 0){
		goto shmget_failed;
	}
	shm = shmat(shmid, 0, 0);
	if(shm < 0){
		goto shmat_failed;
	}
	if(add_memory_to_list(shmid, shm) < 0){
		free_shared_memory(shmid, shm);
		return NULL;
	}
	return shm;
shmat_failed :
	shmctl(shmid, IPC_RMID, 0);
shmget_failed :
	return NULL;
}
void* find_ipc_shm(key_t key, size_t size)
{
	void *shm = NULL;
	int shmid;
	shmid = shmget(key, size, SEMPERM|IPC_CREAT|IPC_EXCL);
	if(shmid < 0){
		shmid = shmget(key, size, SEMPERM|IPC_CREAT);
		if(shmid < 0){
			goto shmget_failed;
		}
		shm = shmat(shmid, 0, 0);
		if(shm < 0){
			goto shmat_failed;
		}
		if(add_memory_to_list(shmid, shm) < 0){
			free_shared_memory(shmid, shm);
			return NULL;
		}
		return shm;
	}else{
		//share memory not exsit
		shmctl(shmid, IPC_RMID, 0);
		goto shmget_failed;
	}
shmat_failed :
	shmctl(shmid, IPC_RMID, 0);
shmget_failed :
	return NULL;
}
static int free_shared_memory(int shmid, void *shm)
{
	if(shmdt(shm) == -1){
		return -1;
	}
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		return -1;
	}
	return 0;
}
static bool find_memory_cb(const void *data, const void *match_data)
{
	struct memory *memory = (struct memory*)data;
	if(memory->shm == match_data){
		return true;
	}else{
		return false;
	}
}
int destroy_ipc_shm(void *shm)
{
	struct memory *memory;
	if((!memory_list) || (!shm)){
		return -1;
	}
	memory = queue_find(memory_list, find_memory_cb, shm);
	if(!memory){
		return -1;
	}
	if(free_shared_memory(memory->shmid, memory->shm)<0){
		return -1;
	}
	queue_remove(memory_list, memory);
	return 0;
}
static int add_memory_to_list(int shmid, void *shm)
{
	struct memory *memory;
	if(!memory_list){
		memory_list = queue_new();
		if(!memory_list){
			return -1;
		}
	}
	memory = mem_malloc(sizeof(struct memory));
	if(!memory){
		return -1;
	}
	memory->shmid = shmid;
	memory->shm = shm;
	if(queue_push_tail(memory_list, memory) == false){
		mem_free(memory);
		return -1;
	}
	return 0;
}
