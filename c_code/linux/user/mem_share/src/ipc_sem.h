#ifndef __IPC_SEM_H__
#define __IPC_SEM_H__

#include <sys/sem.h>
#include "mem_manage.h"

struct ipc_sem {
	key_t key;
	int semid;
	int nsems;
};

struct ipc_sem* ipc_create_sem(key_t key, int nsems);
int ipc_init_sem(struct ipc_sem* sem, int *value);
int ipc_find_sem(struct ipc_sem* sem);
int ipc_destroy_sem(struct ipc_sem* sem);
int ipc_sem_v(int semid, int nsem);
int ipc_sem_p(int semid, int nsem);

#endif /* __IPC_SEM_H__ */
