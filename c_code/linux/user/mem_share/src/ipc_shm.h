#ifndef __IPC_SHM_H__
#define __IPC_SHM_H__

#include <sys/shm.h>

void* create_ipc_shm(key_t key, size_t size);
void* find_ipc_shm(key_t key, size_t size);
int destroy_ipc_shm(void *shm);

#endif /* __IPC_SHM_H__ */
