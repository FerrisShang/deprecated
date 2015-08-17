#ifndef __HCI__EVENT_PROC_H__
#define __HCI__EVENT_PROC_H__

#include "base.h"
#include "proc_common.h"

#define HCI_EVENT_MIN_CODE EVENT_INQUIRY_COMPLETE
#define HCI_EVENT_MAX_CODE EVENT_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED

int bt_hci_event_process(struct bt_pkg *bt_pkg);

#endif /* __HCI__EVENT_PROC_H__ */
