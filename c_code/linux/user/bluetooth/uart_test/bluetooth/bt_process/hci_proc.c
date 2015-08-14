#include <stdio.h>
#include "proc_common.h"
#include "hci_proc.h"
#include "hci_event_proc.h"

static int hci_proc(struct bt_pkg *bt_pkg, void *pdata);
static struct bt_pkg_proc_node proc_node = {
	hci_proc,
	NULL,
};
static struct bt_pkg_proc_list bt_pkg_proc_list = {
	NULL,
	&proc_node,
};
static struct bt_pkg_proc_list bt_pkg_proc_list_head = {
	&bt_pkg_proc_list,
	NULL,
};

int bt_hci_process(struct bt_pkg *bt_pkg)
{
	return bt_pkg_process(&bt_pkg_proc_list_head, bt_pkg);
}

int bt_hci_proc_reg(struct bt_pkg_proc_node *node)
{
	return bt_pkg_proc_node_reg(&bt_pkg_proc_list_head, node);
}

int bt_hci_proc_unreg(struct bt_pkg_proc_node *node)
{
	return bt_pkg_proc_node_unreg(&bt_pkg_proc_list_head, node);
}

static int hci_proc(struct bt_pkg *bt_pkg, void *pdata)
{
	char bt_pkg_type = bt_pkg->pbuf[0];
	int ret;
	bt_pkg->pbuf += HCI_HEADER_LEN;
	bt_pkg->buf_len -= HCI_HEADER_LEN;
	switch(bt_pkg_type){
		case HCI_PKT_TYPE_ACL:
			dbg_print(DBG_WARNING, BT_PROC_DBG, "ACL package processing not implement\n");
			ret = BT_PKG_CB_RET_FAIL;
			break;
		case HCI_PKT_TYPE_SYNC:
			dbg_print(DBG_WARNING, BT_PROC_DBG, "Sync data package processing not implement\n");
			ret = BT_PKG_CB_RET_FAIL;
			break;
		case HCI_PKT_TYPE_EVENT:
			ret = bt_hci_event_process(bt_pkg);
			break;
		default:
			dbg_print(DBG_WARNING, BT_PROC_DBG, "unknown package type found\n");
			ret = BT_PKG_CB_RET_FAIL;
			break;
	}
	return ret;
}
