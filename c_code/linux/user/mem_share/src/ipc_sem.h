#ifndef __IPC_SEM_H__
#define __IPC_SEM_H__

#include <sys/sem.h>

int create_ipc_sem(key_t key, int nsems, int *value);
int find_ipc_sem(key_t key, int nsems);
int destroy_ipc_sem(int semid);
int ipc_sem_v(int semid, int nsems);
int ipc_sem_p(int semid, int nsems);



#endif /* __IPC_SEM_H__ */
