#ifndef __HCI_PROC_H__
#define __HCI_PROC_H__

#include "base.h"
#include "proc_common.h"

int bt_hci_process(struct bt_msg *bt_msg);
int bt_hci_proc_reg(struct bt_msg_proc_node *node);
int bt_hci_proc_unreg(struct bt_msg_proc_node *node);

#endif /* __HCI_PROC_H__ */


