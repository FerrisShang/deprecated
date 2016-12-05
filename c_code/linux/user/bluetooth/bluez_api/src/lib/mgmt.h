#ifndef __MGMT_H__
#define __MGMT_H__

#include "bluetooth.h"
#include "hci.h"

/* IO Capabilities values */
#define SMP_IO_DISPLAY_ONLY 0x00
#define SMP_IO_DISPLAY_YESNO    0x01
#define SMP_IO_KEYBOARD_ONLY    0x02
#define SMP_IO_NO_INPUT_OUTPUT  0x03
#define SMP_IO_KEYBOARD_DISPLAY 0x04

#define MGMT_MAX_NAME_LENGTH        (248 + 1)
#define MGMT_MAX_SHORT_NAME_LENGTH  (10 + 1)

int mgmt_setup(int hdev);
int mgmt_set_iocap(int hdev, uint8_t io_cap);
int mgmt_disconnect(int hdev, bdaddr_t *bdaddr, uint8_t bdaddr_type);
int mgmt_set_name(int hdev, char *name, char *short_name);
#endif /* __MGMT_H__ */
