#ifndef __WI_BUS_H__
#define __WI_BUS_H__

#include <stdint.h>

#define WI_FLAG_PRIORITY_HIGH  (1 << 0)
#define WI_FLAG_PRIORITY_LOW   (0 << 0)
#define WI_FLAG_SEND_BLOCK     (1 << 2)
#define WI_FLAG_SEND_NONBLOCK  (0 << 2)

#define WI_RET_SUCCESS         0
#define WI_RET_FAILED         -1
#define WI_RET_NO_ADDR        -2
#define WI_RET_ADDR_EXSIT     -3

typedef struct {
	uint8_t d[8];
}wiaddr_t;

//wi_bus server api
int wi_bus_server_run();

//wi_bus client api
int wi_register(
		wiaddr_t *local_id,
		void (*recv_cb)(wiaddr_t *remote_id, char *buf, int len, void *user_data),
		void (*disc_cb)(void *user_data),
		void *user_data);
int wi_send(wiaddr_t *remote_id, char *buf, int len, int flag);
int wi_unregister(void);

#endif /* __WI_BUS_H__ */
