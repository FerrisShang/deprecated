#ifndef __WI_BUS_H__
#define __WI_BUS_H__

#include <sys/shm.h>
#include "ipc_sem.h"
#include "ipc_shm.h"

#define FLAG_PRIORITY_HIGH     (1 << 0)
#define FLAG_PRIORITY_LOW      (0 << 0)
#define FLAG_SEND_BLOCK        (1 << 2)
#define FLAG_SEND_NONBLOCK     (0 << 2)

#define MEM_RET_SUCCESS         0
#define MEM_RET_NO_MEM         -1
#define MEM_RET_NO_ADDR        -2
#define MEM_RET_ADDR_EXSIT     -3

#define BROADCASE_ID           0xFFFFFFFF

int mem_send(int local_id, int remote_id, char *buf, int len, int flag);
int mem_register(int local_id,
		void (*callback)(int remote_id, char *buf, int len, void *user_data),
		void *user_data);

#endif /* __WI_BUS_H__ */
