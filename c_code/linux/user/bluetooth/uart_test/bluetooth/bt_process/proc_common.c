#include "base.h"
#include "proc_common.h"

int bt_pkg_process(struct bt_pkg_proc_list *head, struct bt_pkg *bt_pkg)
{
	int ret = BT_PKG_CB_RET_FAIL;
	struct bt_pkg_proc_list *head_bk = head;
	while(1){
		if(head->next == NULL){
			break;
		}else{
			head= head->next;
		}
		if(head->proc_node->func != NULL){
			ret = head->proc_node->func(bt_pkg, head->proc_node->pdata);
			if(ret == BT_PKG_CB_RET_SUC){
				break;
			}else if(ret == BT_PKG_CB_RET_SUC_UNREG){
				bt_pkg_proc_node_unreg(head_bk, head->proc_node);
				break;
			}else if(ret == BT_PKG_CB_RET_FAIL){
				continue;
			}else{
				dbg_print(DBG_WARNING, BT_PROC_DBG, "package process callback return error value.\n");
			}
		}
	}
	return ret;
}

int bt_pkg_proc_node_reg(struct bt_pkg_proc_list *head, struct bt_pkg_proc_node *node)
{
	struct bt_pkg_proc_list *list;
	list = mem_malloc(sizeof(struct bt_pkg_proc_list));
	if(list == NULL){
		dbg_print(DBG_ERROR, BT_PROC_DBG, "malloc error\n");
		return -1;
	}
	list->proc_node = node;
	list->next = head->next;
	head->next = list;
	return 0;
}

int bt_pkg_proc_node_unreg(struct bt_pkg_proc_list *head, struct bt_pkg_proc_node *node)
{
	int ret = -1;
	while(1){
		if(head->next == NULL){
			break;
		}
		if(head->next->proc_node == node){
			head->next = head->next->next;
			ret = 0;
			break;
		}
		head = head->next;
	}
	return ret;
}
