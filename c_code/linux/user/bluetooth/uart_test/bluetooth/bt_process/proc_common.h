#ifndef __PKG_TYPE_H__
#define __PKG_TYPE_H__

#include "hci.h"

enum bt_pkg_type {
	BT_PKG_TYPE_HCI,
	BT_PKG_TYPE_EVENT,
	BT_PKG_TYPE_L2CAP,
	BT_PKG_TYPE_ACL,
	BT_PKG_TYPE_ATT,
};

#define BT_PKG_CB_RET_SUC_UNREG     1
#define BT_PKG_CB_RET_SUC           0
#define BT_PKG_CB_RET_FAIL         -1

struct bt_pkg {
	char *pbuf;
	int buf_len;
	enum bt_pkg_type type;
};

struct bt_pkg_proc_node {
	int (*func)(struct bt_pkg*, void *pdata);
	void *pdata;
};

struct bt_pkg_proc_list {
	struct bt_pkg_proc_list *next;
	struct bt_pkg_proc_node *proc_node;
};

int bt_pkg_process(struct bt_pkg_proc_list *head, struct bt_pkg *bt_pkg);
int bt_pkg_proc_node_reg(struct bt_pkg_proc_list *head, struct bt_pkg_proc_node *node);
int bt_pkg_proc_node_unreg(struct bt_pkg_proc_list *head, struct bt_pkg_proc_node *node);

#endif /* __PKG_TYPE_H__ */
