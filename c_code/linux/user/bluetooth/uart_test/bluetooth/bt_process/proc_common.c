#include "base.h"
#include "proc_common.h"

int bt_msg_proc_node_reg(struct bt_msg_proc_list **head, struct bt_msg_proc_node *node)
{
	struct bt_msg_proc_list *list;
	list = mem_malloc(sizeof(struct bt_msg_proc_list));
	if(list == NULL){
		dbg_print(DBG_ERROR, BT_PROC_DBG, "malloc error\n");
		return -1;
	}
	list->proc_node = node;
	list->next = *head;
	*head = list;
	return 0;
}

int bt_msg_proc_node_unreg(struct bt_msg_proc_list **head, struct bt_msg_proc_node *node)
{
	return 0;
}
