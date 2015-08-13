#ifndef __MSG_TYPE_H__
#define __MSG_TYPE_H__

enum bt_msg_type {
	BT_MSG_TYPE_HCI,
	BT_MSG_TYPE_EVENT,
	BT_MSG_TYPE_L2CAP,
	BT_MSG_TYPE_ACL,
	BT_MSG_TYPE_ATT,
};

enum bt_msg_cb_type {
	BT_MSG_CB_MULTI,
	BT_MSG_CB_ONCE,
};

struct bt_msg {
	char *pbuf;
	int buf_len;
	enum bt_msg_type type;
};

struct bt_msg_proc_node {
	int (*func)(struct bt_msg*);
	enum bt_msg_cb_type func_type;
};

struct bt_msg_proc_list {
	struct bt_msg_proc_list *next;
	struct bt_msg_proc_node *proc_node;
};

int bt_msg_proc_node_reg(struct bt_msg_proc_list **head, struct bt_msg_proc_node *node);
int bt_msg_proc_node_unreg(struct bt_msg_proc_list **head, struct bt_msg_proc_node *node);

#endif /* __MSG_TYPE_H__ */
