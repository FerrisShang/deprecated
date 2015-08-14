#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "bluetooth.h"

//struct bt_proc_node 
int uart_process(char *buf, int len)
{
	struct bt_pkg bt_pkg;
	char *pbuf;
	int ret;
	memset(&bt_pkg, 0, sizeof(bt_pkg));
	pbuf = mem_malloc(len);
	if(pbuf == NULL){
		dbg_print(DBG_ERROR, BT_PROC_DBG, "malloc error\n");
	}
	memcpy(pbuf, buf, len);
	bt_pkg.pbuf = pbuf;
	bt_pkg.buf_len = len;
	bt_pkg.type = BT_PKG_TYPE_HCI;
	ret = bt_hci_process(&bt_pkg);
	if(ret == BT_PKG_CB_RET_FAIL){
		dbg_print(DBG_WARNING, BT_PROC_DBG, "Process bt package failed\n");
	}
	mem_free(pbuf);
	return 0;
}
