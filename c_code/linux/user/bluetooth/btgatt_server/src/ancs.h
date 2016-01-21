#ifndef __ANCS_H__
#define __ANCS_H__

#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"

#include "src/mainloop.h"
#include "src/util.h"
#include "src/att.h"
#include "src/queue.h"
#include "src/gatt-db.h"
#include "src/gatt-client.h"
#include "AncsParser.h"

typedef void (*ancs_func_t)(resp_data_t *getNotifCmd, void *user_data);
int ancs_start(ancs_func_t ancs_cb, char *le_name);

#endif /* __ANCS_H__ */
