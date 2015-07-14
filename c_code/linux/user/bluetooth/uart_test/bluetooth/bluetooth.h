#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#define TYPE_HCI_CMD_DATA   1
#define TYPE_HCI_ACL_DATA   2
#define TYPE_HCI_SYNC_DATA  3
#define TYPE_HCI_EVENT_DATA 4

#define TYPE_EVENT_FUNC_MAX_INDEX 0x3e

struct inBlue{
	;
};

struct data_process{
	int (*func[4])(char *buf, int len, void *data);
};

struct hci_acl_process{
	;
};

struct hci_sync_process{
	;
};

struct hci_event_process{
	int (*func[TYPE_EVENT_FUNC_MAX_INDEX])(char *buf, int len, void *data);
};

#endif /* __BLUETOOTH_H__ */
