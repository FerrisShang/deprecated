#ifndef __MGMT_H__
#define __MGMT_H__

#include "bluetooth.h"
#include "hci.h"
int mgmt_setup(int hdev);
int mgmt_disconnect(int hdev, bdaddr_t *bdaddr, uint8_t bdaddr_type);

#endif /* __MGMT_H__ */
