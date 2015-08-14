#include <stdio.h>
#include "proc_common.h"
#include "hci_proc.h"
#include "hci_event_proc.h"

extern int (*hci_event_func[])(struct bt_pkg *bt_pkg, void *pdata);

static int hci_event_proc(struct bt_pkg *bt_pkg, void *pdata);
static struct bt_pkg_proc_node proc_node = {
	hci_event_proc,
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

int bt_hci_event_process(struct bt_pkg *bt_pkg)
{
	return bt_pkg_process(&bt_pkg_proc_list_head, bt_pkg);
}

int bt_hci_event_proc_reg(struct bt_pkg_proc_node *node)
{
	return bt_pkg_proc_node_reg(&bt_pkg_proc_list_head, node);
}

int bt_hci_event_proc_unreg(struct bt_pkg_proc_node *node)
{
	return bt_pkg_proc_node_unreg(&bt_pkg_proc_list_head, node);
}

static int hci_event_proc(struct bt_pkg *bt_pkg, void *pdata)
{
	int ret = BT_PKG_CB_RET_FAIL;
	unsigned char event_code;
	unsigned char total_len;
	event_code = bt_pkg->pbuf[0];
	total_len = bt_pkg->pbuf[1];
	bt_pkg->pbuf += 2;
	bt_pkg->buf_len -= 2;
	if(total_len != bt_pkg->buf_len){
		dbg_print(DBG_WARNING, BT_PROC_DBG, "package length not correct\n");
		return ret;
	}
	if(event_code >= HCI_EVENT_MIN_CODE && event_code <= HCI_EVENT_MAX_CODE){
		if(hci_event_func[event_code] != NULL){
			ret = hci_event_func[event_code](bt_pkg, pdata);
		}else{
			dbg_print(DBG_WARNING, BT_PROC_DBG, "can't process event:0x%02x\n", event_code);
		}
	}
	return ret;
}

int (*hci_event_func[])(struct bt_pkg *bt_pkg, void *pdata) = {
	/* RESERVE             ... 0x00 */ NULL,
	/* INQUIRY_COMPLETE    ... 0x01 */ NULL,
	/* INQUIRY_RESULT      ... 0x02 */ NULL,
	/* CONNECTION_COMPLETE ... 0x03 */ NULL,
	/* CONNECTION_REQUEST  ... 0x04 */ NULL,
	/* DISCONNECTION_COMPLE... 0x05 */ NULL,
	/* AUTHENTICATION_COMPL... 0x06 */ NULL,
	/* REMOTE_NAME_REQUEST_... 0x07 */ NULL,
	/* ENCRYPTION_CHANGE   ... 0x08 */ NULL,
	/* CHANGE_CONNECTION_LI... 0x09 */ NULL,
	/* MASTER_LINK_KEY_COMP... 0x0a */ NULL,
	/* READ_REMOTE_SUPPORTE... 0x0b */ NULL,
	/* READ_REMOTE_VERSION_... 0x0c */ NULL,
	/* QOS_SETUP_COMPLETE  ... 0x0d */ NULL,
	/* COMMAND_COMPLETE    ... 0x0e */ NULL,
	/* COMMAND_STATUS      ... 0x0f */ NULL,
	/* HARDWARE_ERROR      ... 0x10 */ NULL,
	/* FLUSH_OCCURRED      ... 0x11 */ NULL,
	/* ROLE_CHANGE         ... 0x12 */ NULL,
	/* NUMBER_OF_COMPLETED_... 0x13 */ NULL,
	/* MODE_CHANGE         ... 0x14 */ NULL,
	/* RETURN_LINK_KEYS    ... 0x15 */ NULL,
	/* PIN_CODE_REQUEST    ... 0x16 */ NULL,
	/* LINK_KEY_REQUEST    ... 0x17 */ NULL,
	/* LINK_KEY_NOTIFICATIO... 0x18 */ NULL,
	/* LOOPBACK_COMMAND    ... 0x19 */ NULL,
	/* DATA_BUFFER_OVERFLOW... 0x1a */ NULL,
	/* MAX_SLOTS_CHANGE    ... 0x1b */ NULL,
	/* READ_CLOCK_OFFSET_CO... 0x1c */ NULL,
	/* CONNECTION_PACKET_TY... 0x1d */ NULL,
	/* QOS_VIOLATION       ... 0x1e */ NULL,
	/* RESERVE             ... 0x1f */ NULL,
	/* PAGE_SCAN_REPETITION... 0x20 */ NULL,
	/* FLOW_SPECIFICATION_C... 0x21 */ NULL,
	/* INQUIRY_RESULT_WITH_... 0x22 */ NULL,
	/* READ_REMOTE_EXTENDED... 0x23 */ NULL,
	/* RESERVE             ... 0x24 */ NULL,
	/* RESERVE             ... 0x25 */ NULL,
	/* RESERVE             ... 0x26 */ NULL,
	/* RESERVE             ... 0x27 */ NULL,
	/* RESERVE             ... 0x28 */ NULL,
	/* RESERVE             ... 0x29 */ NULL,
	/* RESERVE             ... 0x2a */ NULL,
	/* RESERVE             ... 0x2b */ NULL,
	/* SYNCHRONOUS_CONNECTI... 0x2c */ NULL,
	/* SYNCHRONOUS_CONNECTI... 0x2d */ NULL,
	/* SNIFF_SUBRATING     ... 0x2e */ NULL,
	/* EXTENDED_INQUIRY_RES... 0x2f */ NULL,
	/* ENCRYPTION_KEY_REFRE... 0x30 */ NULL,
	/* IO_CAPABILITY_REQUES... 0x31 */ NULL,
	/* IO_CAPABILITY_RESPON... 0x32 */ NULL,
	/* USER_CONFIRMATION_RE... 0x33 */ NULL,
	/* USER_PASSKEY_REQUEST... 0x34 */ NULL,
	/* REMOTE_OOB_DATA_REQU... 0x35 */ NULL,
	/* SIMPLE_PAIRING_COMPL... 0x36 */ NULL,
	/* RESERVE             ... 0x37 */ NULL,
	/* LINK_SUPERVISION_TIM... 0x38 */ NULL,
	/* ENHANCED_FLUSH_COMPL... 0x39 */ NULL,
	/* RESERVE             ... 0x3a */ NULL,
	/* USER_PASSKEY_NOTIFIC... 0x3b */ NULL,
	/* KEYPRESS_NOTIFICATIO... 0x3c */ NULL,
	/* REMOTE_HOST_SUPPORTE... 0x3d */ NULL,
	/* LE_META             ... 0x3e */ NULL,
	/* RESERVE             ... 0x3f */ NULL,
	/* PHYSICAL_LINK_COMPLE... 0x40 */ NULL,
	/* CHANNEL_SELECTED    ... 0x41 */ NULL,
	/* DISCONNECTION_PHYSIC... 0x42 */ NULL,
	/* PHYSICAL_LINK_LOSS_E... 0x43 */ NULL,
	/* PHYSICAL_LINK_RECOVE... 0x44 */ NULL,
	/* LOGICAL_LINK_COMPLET... 0x45 */ NULL,
	/* DISCONNECTION_LOGICA... 0x46 */ NULL,
	/* FLOW_SPEC_MODIFY_COM... 0x47 */ NULL,
	/* NUMBER_OF_COMPLETED_... 0x48 */ NULL,
	/* AMP_START_TEST      ... 0x49 */ NULL,
	/* AMP_TEST_END        ... 0x4a */ NULL,
	/* AMP_RECEIVER_REPORT ... 0x4b */ NULL,
	/* SHORT_RANGE_MODE_CHA... 0x4c */ NULL,
	/* AMP_STATUS_CHANGE   ... 0x4d */ NULL,
	/* TRIGGERED_CLOCK_CAPT... 0x4e */ NULL,
	/* SYNCHRONIZATION_TRAI... 0x4f */ NULL,
	/* SYNCHRONIZATION_TRAI... 0x50 */ NULL,
	/* CONNECTIONLESS_SLAVE... 0x51 */ NULL,
	/* CONNECTIONLESS_SLAVE... 0x52 */ NULL,
	/* TRUNCATED_PAGE_COMPL... 0x53 */ NULL,
	/* SLAVE_PAGE_RESPONSE_... 0x54 */ NULL,
	/* CONNECTIONLESS_SLAVE... 0x55 */ NULL,
	/* INQUIRY_RESPONSE_NOT... 0x56 */ NULL,
	/* AUTHENTICATED_PAYLOA... 0x57 */ NULL,
};
