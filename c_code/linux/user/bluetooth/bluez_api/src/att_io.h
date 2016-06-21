#ifndef __ATT_IO_H__
#define __ATT_IO_H__
#include "bluetooth.h"
#include "types.h"
enum{
	ATT_IO_STATUS_CONNECTED,
	ATT_IO_STATUS_DISCONNECTED,
};
enum{
	ATT_IO_SUCCESS         = 0,
	ATT_IO_FAILED_BUSY     = -1,
	ATT_IO_FAILED_NOEXIST  = -2,
};
struct att_io_cb {
	void (*conn_change_cb)(bdaddr_t addr, int status, void *pdata);
	void (*receive)(bdaddr_t addr, UINT8 *dat, UINT32 len, void *pdata);
};
struct att_io {
	int (*connect)(bdaddr_t addr);
	int (*send)(bdaddr_t addr, UINT8 *dat, UINT32 len);
};
const struct att_io* register_att_io(int hdev, struct att_io_cb *io_cb, void *pdata);

#endif /* __ATT_IO_H__ */
