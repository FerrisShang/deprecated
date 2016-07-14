#ifndef __IPC_SEM_H__
#define __IPC_SEM_H__

#include <sys/shm.h>

void* create_ipc_sem(key_t key, size_t size);
void* find_ipc_sem(key_t key, size_t size);
int destroy_ipc_sem(void *shm);



#endif /* __IPC_SEM_H__ */
