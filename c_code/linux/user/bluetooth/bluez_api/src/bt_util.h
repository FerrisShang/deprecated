#ifndef __BT_UTIL_H__
#define __BT_UTIL_H__
#include "types.h"
#include "bluetooth.h"
#include "mgmt.h"
#include "hci.h"

int l2cap_le_att_listen_and_accept(int hdev, int security,
		void (*connected_cb)(int fd, bdaddr_t addr));
int l2cap_le_att_connect(int hdev, bdaddr_t *dst, int security,
		void (*connected_cb)(int fd, bdaddr_t addr));
int le_set_random_address(int hdev);
int le_set_advertise_enable(int hdev);
int le_set_advertise_parameters(int hdev);
void le_disconnect(int hdev, bdaddr_t *addr);
void dump_btaddr(char *prefix, bdaddr_t *addr);

#endif /* __BT_UTIL_H__ */
