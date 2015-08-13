#include <stdio.h>
#include "proc_common.h"
#include "hci_proc.h"

static int hci_proc(struct bt_msg *bt_msg);

struct bt_msg_proc_node proc_node = {
	hci_proc,
	BT_MSG_CB_MULTI,
};

static struct bt_msg_proc_list bt_msg_proc_list = {
	NULL,
	&proc_node,
};

static struct bt_msg_proc_list *bt_msg_proc_list_head = &bt_msg_proc_list;

int bt_hci_process(struct bt_msg *bt_msg)
{
	struct bt_msg_proc_list *proc_list;
	int ret = -1;

	proc_list = bt_msg_proc_list_head;
	do{
		if(proc_list->proc_node->func != NULL){
			ret = proc_list->proc_node->func(bt_msg);
			if(ret == 0){
				if(proc_list->proc_node->func_type == BT_MSG_CB_ONCE){
					bt_hci_proc_unreg(proc_list->proc_node);
				}
				break;
			}
		}
		if(proc_list->next == NULL){
			break;
		}else{
			proc_list = proc_list->next;
		}
	}while(ret != 0);
	//proc_node maybe already free, we cannot access it below
	if(ret != 0){
		dbg_print(DBG_WARNING, BT_PROC_DBG, "process hci message failed\n");
	}
	return ret;
}

int bt_hci_proc_reg(struct bt_msg_proc_node *node)
{
	return bt_msg_proc_node_reg(&bt_msg_proc_list_head, node);
}

int bt_hci_proc_unreg(struct bt_msg_proc_node *node)
{
	return bt_msg_proc_node_unreg(&bt_msg_proc_list_head, node);
}

static int hci_proc(struct bt_msg *bt_msg)
{
	printf("hci received, buflen = %d\n", bt_msg->buf_len);
	return 0;
}
