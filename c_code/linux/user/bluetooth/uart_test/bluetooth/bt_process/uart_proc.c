#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "bluetooth.h"

//struct bt_proc_node 
int uart_process(char *buf, int len)
{
	struct bt_msg bt_msg;
	char *pbuf;
	memset(&bt_msg, 0, sizeof(bt_msg));
	pbuf = mem_malloc(len);
	if(pbuf == NULL){
		dbg_print(DBG_ERROR, BT_PROC_DBG, "malloc error\n");
	}
	bt_msg.pbuf = pbuf;
	bt_msg.buf_len = len;
	bt_msg.type = BT_MSG_TYPE_HCI;
	bt_hci_process(&bt_msg);
	mem_free(pbuf);
	return 0;
}
