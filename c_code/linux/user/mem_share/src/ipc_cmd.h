#ifndef __IPC_CMD_H__
#define __IPC_CMD_H__

#include "ipc_shm.h"
#include "ipc_sem.h"

#define IPC_CMD_BUF_LEN 1024

struct ipc_cmd {
	struct ipc_sem *sem;
	struct ipc_shm *shm;
	void *buf;
	void *pdata;
};

typedef void (*ipc_cmd_io_cb)(void *buf, void *pdata);

struct ipc_cmd* ipc_create_cmd(key_t sem_key, key_t shm_key, void *pdata);
int ipc_cmd_remote_init(struct ipc_cmd *ipc_cmd, void *pdata);
int ipc_send_cmd(struct ipc_cmd *ipc_cmd,
		ipc_cmd_io_cb request_cb, ipc_cmd_io_cb response_cb);
int ipc_recv_cmd(struct ipc_cmd *ipc_cmd, ipc_cmd_io_cb callback);
int ipc_destroy_cmd(struct ipc_cmd *ipc_cmd);

#endif /* __IPC_CMD_H__ */
