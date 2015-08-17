#ifndef __HCI_PROC_H__
#define __HCI_PROC_H__

#include "base.h"
#include "proc_common.h"

#define HCI_PKT_TYPE_ACL   2
#define HCI_PKT_TYPE_SYNC  3
#define HCI_PKT_TYPE_EVENT 4

#define HCI_HEADER_LEN 1

int bt_hci_process(struct bt_pkg *bt_pkg);
int bt_hci_proc_reg(struct bt_pkg_proc_node *node);
int bt_hci_proc_unreg(struct bt_pkg_proc_node *node);

#endif /* __HCI_PROC_H__ */
