#include "base.h"
#include "bluetooth.h"

int hci_data_process(char buf, int len, void *data)
{
	//not implement yet
	dbg_print(DBG_ERROR, BLUETOOTH_DBG, "not implement yet\n");
	return -1;
}

int hci_cmd_process(char buf, int len, void *data)
{
	//Under the normal condition, we'll not receive hci command
	return 0;
}

int hci_acl_process(char buf, int len, void *data)
{
	//not implement yet
	dbg_print(DBG_ERROR, BLUETOOTH_DBG, "not implement yet\n");
	return -1;
}

int hci_sync_process(char buf, int len, void *data)
{
	//not implement yet
	dbg_print(DBG_ERROR, BLUETOOTH_DBG, "not implement yet\n");
	return -1;
}

int hci_event_process(char buf, int len, void *data)
{
	return -1;
}
