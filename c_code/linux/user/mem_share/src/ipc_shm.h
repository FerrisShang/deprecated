#ifndef __IPC_SHM_H__
#define __IPC_SHM_H__

#include <sys/shm.h>
#include "mem_manage.h"

struct ipc_shm {
	key_t key;
	int shmid;
	void *buf;
	size_t size;
};

struct ipc_shm* ipc_create_shm(key_t key, size_t size);
int ipc_destroy_shm(struct ipc_shm *ipc_shm);
int ipc_find_shm_by_id(struct ipc_shm *shm);
int ipc_find_shm_by_key(struct ipc_shm *shm);
int ipc_detach_shm(struct ipc_shm *ipc_shm);

#endif /* __IPC_SHM_H__ */
