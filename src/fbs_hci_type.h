#ifndef FBS_HCI_TYPE_H__
#define FBS_HCI_TYPE_H__

#include "fbs_broadcom_hci.h"

/* -----  HCI Commands ----- */

/* Link Control */
#define OGF_LINK_CTL		0x01

#define OCF_INQUIRY			0x0001
typedef struct {
	guint8		lap[3];
	guint8		length;		/* 1.28s units */
	guint8		num_rsp;
} __attribute__ ((packed)) tFBS_inquiry_cp;
#define INQUIRY_CP_SIZE 5

typedef struct {
	guint8		status;
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_status_bdaddr_rp;
#define STATUS_BDADDR_RP_SIZE 7

#define OCF_INQUIRY_CANCEL		0x0002

#define OCF_PERIODIC_INQUIRY		0x0003
typedef struct {
	guint16	max_period;	/* 1.28s units */
	guint16	min_period;	/* 1.28s units */
	guint8		lap[3];
	guint8		length;		/* 1.28s units */
	guint8		num_rsp;
} __attribute__ ((packed)) tFBS_periodic_inquiry_cp;
#define PERIODIC_INQUIRY_CP_SIZE 9

#define OCF_EXIT_PERIODIC_INQUIRY	0x0004

#define OCF_CREATE_CONN			0x0005
typedef struct {
	bdaddr_t	bdaddr;
	guint16	pkt_type;
	guint8		pscan_rep_mode;
	guint8		pscan_mode;
	guint16	clock_offset;
	guint8		role_switch;
} __attribute__ ((packed)) tFBS_create_conn_cp;
#define CREATE_CONN_CP_SIZE 13

#define OCF_DISCONNECT			0x0006
typedef struct {
	guint16	handle;
	guint8		reason;
} __attribute__ ((packed)) tFBS_disconnect_cp;
#define DISCONNECT_CP_SIZE 3

#define OCF_ADD_SCO			0x0007
typedef struct {
	guint16	handle;
	guint16	pkt_type;
} __attribute__ ((packed)) tFBS_add_sco_cp;
#define ADD_SCO_CP_SIZE 4

#define OCF_CREATE_CONN_CANCEL		0x0008
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_create_conn_cancel_cp;
#define CREATE_CONN_CANCEL_CP_SIZE 6

#define OCF_ACCEPT_CONN_REQ		0x0009
typedef struct {
	bdaddr_t	bdaddr;
	guint8		role;
} __attribute__ ((packed)) tFBS_accept_conn_req_cp;
#define ACCEPT_CONN_REQ_CP_SIZE	7

#define OCF_REJECT_CONN_REQ		0x000A
typedef struct {
	bdaddr_t	bdaddr;
	guint8		reason;
} __attribute__ ((packed)) tFBS_reject_conn_req_cp;
#define REJECT_CONN_REQ_CP_SIZE	7

#define OCF_LINK_KEY_REPLY		0x000B
typedef struct {
	bdaddr_t	bdaddr;
	guint8		link_key[16];
} __attribute__ ((packed)) tFBS_link_key_reply_cp;
#define LINK_KEY_REPLY_CP_SIZE 22

#define OCF_LINK_KEY_NEG_REPLY		0x000C

#define OCF_PIN_CODE_REPLY		0x000D
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pin_len;
	guint8		pin_code[16];
} __attribute__ ((packed)) tFBS_pin_code_reply_cp;
#define PIN_CODE_REPLY_CP_SIZE 23

#define OCF_PIN_CODE_NEG_REPLY		0x000E

#define OCF_SET_CONN_PTYPE		0x000F
typedef struct {
	guint16	 handle;
	guint16	 pkt_type;
} __attribute__ ((packed)) tFBS_set_conn_ptype_cp;
#define SET_CONN_PTYPE_CP_SIZE 4

#define OCF_AUTH_REQUESTED		0x0011
typedef struct {
	guint16	 handle;
} __attribute__ ((packed)) tFBS_auth_requested_cp;
#define AUTH_REQUESTED_CP_SIZE 2

#define OCF_SET_CONN_ENCRYPT		0x0013
typedef struct {
	guint16	handle;
	guint8		encrypt;
} __attribute__ ((packed)) tFBS_set_conn_encrypt_cp;
#define SET_CONN_ENCRYPT_CP_SIZE 3

#define OCF_CHANGE_CONN_LINK_KEY	0x0015
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_change_conn_link_key_cp;
#define CHANGE_CONN_LINK_KEY_CP_SIZE 2

#define OCF_MASTER_LINK_KEY		0x0017
typedef struct {
	guint8		key_flag;
} __attribute__ ((packed)) tFBS_master_link_key_cp;
#define MASTER_LINK_KEY_CP_SIZE 1

#define OCF_REMOTE_NAME_REQ		0x0019
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pscan_rep_mode;
	guint8		pscan_mode;
	guint16	clock_offset;
} __attribute__ ((packed)) tFBS_remote_name_req_cp;
#define REMOTE_NAME_REQ_CP_SIZE 10

#define OCF_REMOTE_NAME_REQ_CANCEL	0x001A
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_remote_name_req_cancel_cp;
#define REMOTE_NAME_REQ_CANCEL_CP_SIZE 6

#define OCF_READ_REMOTE_FEATURES	0x001B
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_read_remote_features_cp;
#define READ_REMOTE_FEATURES_CP_SIZE 2

#define OCF_READ_REMOTE_EXT_FEATURES	0x001C
typedef struct {
	guint16	handle;
	guint8		page_num;
} __attribute__ ((packed)) tFBS_read_remote_ext_features_cp;
#define READ_REMOTE_EXT_FEATURES_CP_SIZE 3

#define OCF_READ_REMOTE_VERSION		0x001D
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_read_remote_version_cp;
#define READ_REMOTE_VERSION_CP_SIZE 2

#define OCF_READ_CLOCK_OFFSET		0x001F
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_read_clock_offset_cp;
#define READ_CLOCK_OFFSET_CP_SIZE 2

#define OCF_READ_LMP_HANDLE		0x0020

#define OCF_SETUP_SYNC_CONN		0x0028
typedef struct {
	guint16	handle;
	guint32	tx_bandwith;
	guint32	rx_bandwith;
	guint16	max_latency;
	guint16	voice_setting;
	guint8		retrans_effort;
	guint16	pkt_type;
} __attribute__ ((packed)) tFBS_setup_sync_conn_cp;
#define SETUP_SYNC_CONN_CP_SIZE 17

#define OCF_ACCEPT_SYNC_CONN_REQ	0x0029
typedef struct {
	bdaddr_t	bdaddr;
	guint32	tx_bandwith;
	guint32	rx_bandwith;
	guint16	max_latency;
	guint16	voice_setting;
	guint8		retrans_effort;
	guint16	pkt_type;
} __attribute__ ((packed)) tFBS_accept_sync_conn_req_cp;
#define ACCEPT_SYNC_CONN_REQ_CP_SIZE 21

#define OCF_REJECT_SYNC_CONN_REQ	0x002A
typedef struct {
	bdaddr_t	bdaddr;
	guint8		reason;
} __attribute__ ((packed)) tFBS_reject_sync_conn_req_cp;
#define REJECT_SYNC_CONN_REQ_CP_SIZE 7

#define OCF_IO_CAPABILITY_REPLY		0x002B
typedef struct {
	bdaddr_t	bdaddr;
	guint8		capability;
	guint8		oob_data;
	guint8		authentication;
} __attribute__ ((packed)) tFBS_io_capability_reply_cp;
#define IO_CAPABILITY_REPLY_CP_SIZE 9

#define OCF_USER_CONFIRM_REPLY		0x002C
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_user_confirm_reply_cp;
#define USER_CONFIRM_REPLY_CP_SIZE 6

#define OCF_USER_CONFIRM_NEG_REPLY	0x002D

#define OCF_USER_PASSKEY_REPLY		0x002E
typedef struct {
	bdaddr_t	bdaddr;
	guint32	passkey;
} __attribute__ ((packed)) tFBS_user_passkey_reply_cp;
#define USER_PASSKEY_REPLY_CP_SIZE 10

#define OCF_USER_PASSKEY_NEG_REPLY	0x002F

#define OCF_REMOTE_OOB_DATA_REPLY	0x0030
typedef struct {
	bdaddr_t	bdaddr;
	guint8		hash[16];
	guint8		randomizer[16];
} __attribute__ ((packed)) tFBS_remote_oob_data_reply_cp;
#define REMOTE_OOB_DATA_REPLY_CP_SIZE 38

#define OCF_REMOTE_OOB_DATA_NEG_REPLY	0x0033

#define OCF_IO_CAPABILITY_NEG_REPLY	0x0034
typedef struct {
	bdaddr_t	bdaddr;
	guint8		reason;
} __attribute__ ((packed)) tFBS_io_capability_neg_reply_cp;
#define IO_CAPABILITY_NEG_REPLY_CP_SIZE 7

#define OCF_CREATE_PHYSICAL_LINK		0x0035
typedef struct {
	guint8		handle;
	guint8		key_length;
	guint8		key_type;
	guint8		key[32];
} __attribute__ ((packed)) tFBS_create_physical_link_cp;
#define CREATE_PHYSICAL_LINK_CP_SIZE 35

#define OCF_ACCEPT_PHYSICAL_LINK		0x0036
typedef struct {
	guint8		handle;
	guint8		key_length;
	guint8		key_type;
	guint8		key[32];
} __attribute__ ((packed)) tFBS_accept_physical_link_cp;
#define ACCEPT_PHYSICAL_LINK_CP_SIZE 35

#define OCF_DISCONNECT_PHYSICAL_LINK		0x0037
typedef struct {
	guint8		handle;
	guint8		reason;
} __attribute__ ((packed)) tFBS_disconnect_physical_link_cp;
#define DISCONNECT_PHYSICAL_LINK_CP_SIZE 2

#define OCF_CREATE_LOGICAL_LINK		0x0038
typedef struct {
	guint8		handle;
	guint8		tx_flow[16];
	guint8		rx_flow[16];
} __attribute__ ((packed)) tFBS_create_logical_link_cp;
#define CREATE_LOGICAL_LINK_CP_SIZE 33

#define OCF_ACCEPT_LOGICAL_LINK		0x0039

#define OCF_DISCONNECT_LOGICAL_LINK		0x003A
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_disconnect_logical_link_cp;
#define DISCONNECT_LOGICAL_LINK_CP_SIZE 2

#define OCF_LOGICAL_LINK_CANCEL		0x003B
typedef struct {
	guint8		handle;
	guint8		tx_flow_id;
} __attribute__ ((packed)) tFBS_cancel_logical_link_cp;
#define LOGICAL_LINK_CANCEL_CP_SIZE 2
typedef struct {
	guint8		status;
	guint8		handle;
	guint8		tx_flow_id;
} __attribute__ ((packed)) tFBS_cancel_logical_link_rp;
#define LOGICAL_LINK_CANCEL_RP_SIZE 3

#define OCF_FLOW_SPEC_MODIFY		0x003C

/* Link Policy */
#define OGF_LINK_POLICY		0x02

#define OCF_HOLD_MODE			0x0001
typedef struct {
	guint16	handle;
	guint16	max_interval;
	guint16	min_interval;
} __attribute__ ((packed)) tFBS_hold_mode_cp;
#define HOLD_MODE_CP_SIZE 6

#define OCF_SNIFF_MODE			0x0003
typedef struct {
	guint16	handle;
	guint16	max_interval;
	guint16	min_interval;
	guint16	attempt;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_sniff_mode_cp;
#define SNIFF_MODE_CP_SIZE 10

#define OCF_EXIT_SNIFF_MODE		0x0004
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_exit_sniff_mode_cp;
#define EXIT_SNIFF_MODE_CP_SIZE 2

#define OCF_PARK_MODE			0x0005
typedef struct {
	guint16	handle;
	guint16	max_interval;
	guint16	min_interval;
} __attribute__ ((packed)) tFBS_park_mode_cp;
#define PARK_MODE_CP_SIZE 6

#define OCF_EXIT_PARK_MODE		0x0006
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_exit_park_mode_cp;
#define EXIT_PARK_MODE_CP_SIZE 2

#define OCF_QOS_SETUP			0x0007
typedef struct {
	guint8		service_type;		/* 1 = best effort */
	guint32	token_rate;		/* Byte per seconds */
	guint32	peak_bandwidth;		/* Byte per seconds */
	guint32	latency;		/* Microseconds */
	guint32	delay_variation;	/* Microseconds */
} __attribute__ ((packed)) tFBS_hci_qos;
#define HCI_QOS_CP_SIZE 17
typedef struct {
	guint16	handle;
	guint8		flags;			/* Reserved */
	tFBS_hci_qos		qos;
} __attribute__ ((packed)) tFBS_qos_setup_cp;
#define QOS_SETUP_CP_SIZE (3 + HCI_QOS_CP_SIZE)

#define OCF_ROLE_DISCOVERY		0x0009
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_role_discovery_cp;
#define ROLE_DISCOVERY_CP_SIZE 2
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		role;
} __attribute__ ((packed)) tFBS_role_discovery_rp;
#define ROLE_DISCOVERY_RP_SIZE 4

#define OCF_SWITCH_ROLE			0x000B
typedef struct {
	bdaddr_t	bdaddr;
	guint8		role;
} __attribute__ ((packed)) tFBS_switch_role_cp;
#define SWITCH_ROLE_CP_SIZE 7

#define OCF_READ_LINK_POLICY		0x000C
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_read_link_policy_cp;
#define READ_LINK_POLICY_CP_SIZE 2
typedef struct {
	guint8		status;
	guint16	handle;
	guint16	policy;
} __attribute__ ((packed)) tFBS_read_link_policy_rp;
#define READ_LINK_POLICY_RP_SIZE 5

#define OCF_WRITE_LINK_POLICY		0x000D
typedef struct {
	guint16	handle;
	guint16	policy;
} __attribute__ ((packed)) tFBS_write_link_policy_cp;
#define WRITE_LINK_POLICY_CP_SIZE 4
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_write_link_policy_rp;
#define WRITE_LINK_POLICY_RP_SIZE 3

#define OCF_READ_DEFAULT_LINK_POLICY	0x000E

#define OCF_WRITE_DEFAULT_LINK_POLICY	0x000F

#define OCF_FLOW_SPECIFICATION		0x0010

#define OCF_SNIFF_SUBRATING		0x0011
typedef struct {
	guint16	handle;
	guint16	max_latency;
	guint16	min_remote_timeout;
	guint16	min_local_timeout;
} __attribute__ ((packed)) tFBS_sniff_subrating_cp;
#define SNIFF_SUBRATING_CP_SIZE 8

/* Host Controller and Baseband */
#define OGF_HOST_CTL		0x03

#define OCF_SET_EVENT_MASK		0x0001
typedef struct {
	guint8		mask[8];
} __attribute__ ((packed)) tFBS_set_event_mask_cp;
#define SET_EVENT_MASK_CP_SIZE 8

#define OCF_RESET			0x0003

#define OCF_SET_EVENT_FLT		0x0005
typedef struct {
	guint8		flt_type;
	guint8		cond_type;
	guint8		condition[0];
} __attribute__ ((packed)) tFBS_set_event_flt_cp;
#define SET_EVENT_FLT_CP_SIZE 2

/* Filter types */
#define FLT_CLEAR_ALL			0x00
#define FLT_INQ_RESULT			0x01
#define FLT_CONN_SETUP			0x02
/* INQ_RESULT Condition types */
#define INQ_RESULT_RETURN_ALL		0x00
#define INQ_RESULT_RETURN_CLASS		0x01
#define INQ_RESULT_RETURN_BDADDR	0x02
/* CONN_SETUP Condition types */
#define CONN_SETUP_ALLOW_ALL		0x00
#define CONN_SETUP_ALLOW_CLASS		0x01
#define CONN_SETUP_ALLOW_BDADDR		0x02
/* CONN_SETUP Conditions */
#define CONN_SETUP_AUTO_OFF		0x01
#define CONN_SETUP_AUTO_ON		0x02

#define OCF_FLUSH			0x0008

#define OCF_READ_PIN_TYPE		0x0009
typedef struct {
	guint8		status;
	guint8		pin_type;
} __attribute__ ((packed)) tFBS_read_pin_type_rp;
#define READ_PIN_TYPE_RP_SIZE 2

#define OCF_WRITE_PIN_TYPE		0x000A
typedef struct {
	guint8		pin_type;
} __attribute__ ((packed)) tFBS_write_pin_type_cp;
#define WRITE_PIN_TYPE_CP_SIZE 1

#define OCF_CREATE_NEW_UNIT_KEY		0x000B

#define OCF_READ_STORED_LINK_KEY	0x000D
typedef struct {
	bdaddr_t	bdaddr;
	guint8		read_all;
} __attribute__ ((packed)) tFBS_read_stored_link_key_cp;
#define READ_STORED_LINK_KEY_CP_SIZE 7
typedef struct {
	guint8		status;
	guint16	max_keys;
	guint16	num_keys;
} __attribute__ ((packed)) tFBS_read_stored_link_key_rp;
#define READ_STORED_LINK_KEY_RP_SIZE 5

#define OCF_WRITE_STORED_LINK_KEY	0x0011
typedef struct {
	guint8		num_keys;
	/* variable length part */
} __attribute__ ((packed)) tFBS_write_stored_link_key_cp;
#define WRITE_STORED_LINK_KEY_CP_SIZE 1
typedef struct {
	guint8		status;
	guint8		num_keys;
} __attribute__ ((packed)) tFBS_write_stored_link_key_rp;
#define READ_WRITE_LINK_KEY_RP_SIZE 2

#define OCF_DELETE_STORED_LINK_KEY	0x0012
typedef struct {
	bdaddr_t	bdaddr;
	guint8		delete_all;
} __attribute__ ((packed)) tFBS_delete_stored_link_key_cp;
#define DELETE_STORED_LINK_KEY_CP_SIZE 7
typedef struct {
	guint8		status;
	guint16	num_keys;
} __attribute__ ((packed)) tFBS_delete_stored_link_key_rp;
#define DELETE_STORED_LINK_KEY_RP_SIZE 3

#define HCI_MAX_NAME_LENGTH		248

#define OCF_CHANGE_LOCAL_NAME		0x0013
typedef struct {
	guint8		name[HCI_MAX_NAME_LENGTH];
} __attribute__ ((packed)) tFBS_change_local_name_cp;
#define CHANGE_LOCAL_NAME_CP_SIZE 248

#define OCF_READ_LOCAL_NAME		0x0014
typedef struct {
	guint8		status;
	guint8		name[HCI_MAX_NAME_LENGTH];
} __attribute__ ((packed)) tFBS_read_local_name_rp;
#define READ_LOCAL_NAME_RP_SIZE 249

#define OCF_READ_CONN_ACCEPT_TIMEOUT	0x0015
typedef struct {
	guint8		status;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_read_conn_accept_timeout_rp;
#define READ_CONN_ACCEPT_TIMEOUT_RP_SIZE 3

#define OCF_WRITE_CONN_ACCEPT_TIMEOUT	0x0016
typedef struct {
	guint16	timeout;
} __attribute__ ((packed)) tFBS_write_conn_accept_timeout_cp;
#define WRITE_CONN_ACCEPT_TIMEOUT_CP_SIZE 2

#define OCF_READ_PAGE_TIMEOUT		0x0017
typedef struct {
	guint8		status;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_read_page_timeout_rp;
#define READ_PAGE_TIMEOUT_RP_SIZE 3

#define OCF_WRITE_PAGE_TIMEOUT		0x0018
typedef struct {
	guint16	timeout;
} __attribute__ ((packed)) tFBS_write_page_timeout_cp;
#define WRITE_PAGE_TIMEOUT_CP_SIZE 2

#define OCF_READ_SCAN_ENABLE		0x0019
typedef struct {
	guint8		status;
	guint8		enable;
} __attribute__ ((packed)) tFBS_read_scan_enable_rp;
#define READ_SCAN_ENABLE_RP_SIZE 2

#define OCF_WRITE_SCAN_ENABLE		0x001A
	#define SCAN_DISABLED		0x00
	#define SCAN_INQUIRY		0x01
	#define SCAN_PAGE		0x02

#define OCF_READ_PAGE_ACTIVITY		0x001B
typedef struct {
	guint8		status;
	guint16	interval;
	guint16	window;
} __attribute__ ((packed)) tFBS_read_page_activity_rp;
#define READ_PAGE_ACTIVITY_RP_SIZE 5

#define OCF_WRITE_PAGE_ACTIVITY		0x001C
typedef struct {
	guint16	interval;
	guint16	window;
} __attribute__ ((packed)) tFBS_write_page_activity_cp;
#define WRITE_PAGE_ACTIVITY_CP_SIZE 4

#define OCF_READ_INQ_ACTIVITY		0x001D
typedef struct {
	guint8		status;
	guint16	interval;
	guint16	window;
} __attribute__ ((packed)) tFBS_read_inq_activity_rp;
#define READ_INQ_ACTIVITY_RP_SIZE 5

#define OCF_WRITE_INQ_ACTIVITY		0x001E
typedef struct {
	guint16	interval;
	guint16	window;
} __attribute__ ((packed)) tFBS_write_inq_activity_cp;
#define WRITE_INQ_ACTIVITY_CP_SIZE 4

#define OCF_READ_AUTH_ENABLE		0x001F

#define OCF_WRITE_AUTH_ENABLE		0x0020
	#define AUTH_DISABLED		0x00
	#define AUTH_ENABLED		0x01

#define OCF_READ_ENCRYPT_MODE		0x0021

#define OCF_WRITE_ENCRYPT_MODE		0x0022
	#define ENCRYPT_DISABLED	0x00
	#define ENCRYPT_P2P		0x01
	#define ENCRYPT_BOTH		0x02

#define OCF_READ_CLASS_OF_DEV		0x0023
typedef struct {
	guint8		status;
	guint8		dev_class[3];
} __attribute__ ((packed)) tFBS_read_class_of_dev_rp;
#define READ_CLASS_OF_DEV_RP_SIZE 4

#define OCF_WRITE_CLASS_OF_DEV		0x0024
typedef struct {
	guint8		dev_class[3];
} __attribute__ ((packed)) tFBS_write_class_of_dev_cp;
#define WRITE_CLASS_OF_DEV_CP_SIZE 3

#define OCF_READ_VOICE_SETTING		0x0025
typedef struct {
	guint8		status;
	guint16	voice_setting;
} __attribute__ ((packed)) tFBS_read_voice_setting_rp;
#define READ_VOICE_SETTING_RP_SIZE 3

#define OCF_WRITE_VOICE_SETTING		0x0026
typedef struct {
	guint16	voice_setting;
} __attribute__ ((packed)) tFBS_write_voice_setting_cp;
#define WRITE_VOICE_SETTING_CP_SIZE 2

#define OCF_READ_AUTOMATIC_FLUSH_TIMEOUT	0x0027

#define OCF_WRITE_AUTOMATIC_FLUSH_TIMEOUT	0x0028

#define OCF_READ_NUM_BROADCAST_RETRANS	0x0029

#define OCF_WRITE_NUM_BROADCAST_RETRANS	0x002A

#define OCF_READ_HOLD_MODE_ACTIVITY	0x002B

#define OCF_WRITE_HOLD_MODE_ACTIVITY	0x002C

#define OCF_READ_TRANSMIT_POWER_LEVEL	0x002D
typedef struct {
	guint16	handle;
	guint8		type;
} __attribute__ ((packed)) tFBS_read_transmit_power_level_cp;
#define READ_TRANSMIT_POWER_LEVEL_CP_SIZE 3
typedef struct {
	guint8		status;
	guint16	handle;
	int8_t		level;
} __attribute__ ((packed)) tFBS_read_transmit_power_level_rp;
#define READ_TRANSMIT_POWER_LEVEL_RP_SIZE 4

#define OCF_READ_SYNC_FLOW_ENABLE	0x002E

#define OCF_WRITE_SYNC_FLOW_ENABLE	0x002F

#define OCF_SET_CONTROLLER_TO_HOST_FC	0x0031

#define OCF_HOST_BUFFER_SIZE		0x0033
typedef struct {
	guint16	acl_mtu;
	guint8		sco_mtu;
	guint16	acl_max_pkt;
	guint16	sco_max_pkt;
} __attribute__ ((packed)) tFBS_host_buffer_size_cp;
#define HOST_BUFFER_SIZE_CP_SIZE 7

#define OCF_HOST_NUM_COMP_PKTS		0x0035
typedef struct {
	guint8		num_hndl;
	/* variable length part */
} __attribute__ ((packed)) tFBS_host_num_comp_pkts_cp;
#define HOST_NUM_COMP_PKTS_CP_SIZE 1

#define OCF_READ_LINK_SUPERVISION_TIMEOUT	0x0036
typedef struct {
	guint8		status;
	guint16	handle;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_read_link_supervision_timeout_rp;
#define READ_LINK_SUPERVISION_TIMEOUT_RP_SIZE 5

#define OCF_WRITE_LINK_SUPERVISION_TIMEOUT	0x0037
typedef struct {
	guint16	handle;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_write_link_supervision_timeout_cp;
#define WRITE_LINK_SUPERVISION_TIMEOUT_CP_SIZE 4
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_write_link_supervision_timeout_rp;
#define WRITE_LINK_SUPERVISION_TIMEOUT_RP_SIZE 3

#define OCF_READ_NUM_SUPPORTED_IAC	0x0038

#define MAX_IAC_LAP 0x40
#define OCF_READ_CURRENT_IAC_LAP	0x0039
typedef struct {
	guint8		status;
	guint8		num_current_iac;
	guint8		lap[MAX_IAC_LAP][3];
} __attribute__ ((packed)) tFBS_read_current_iac_lap_rp;
#define READ_CURRENT_IAC_LAP_RP_SIZE 2+3*MAX_IAC_LAP

#define OCF_WRITE_CURRENT_IAC_LAP	0x003A
typedef struct {
	guint8		num_current_iac;
	guint8		lap[MAX_IAC_LAP][3];
} __attribute__ ((packed)) tFBS_write_current_iac_lap_cp;
#define WRITE_CURRENT_IAC_LAP_CP_SIZE 1+3*MAX_IAC_LAP

#define OCF_READ_PAGE_SCAN_PERIOD_MODE	0x003B

#define OCF_WRITE_PAGE_SCAN_PERIOD_MODE	0x003C

#define OCF_READ_PAGE_SCAN_MODE		0x003D

#define OCF_WRITE_PAGE_SCAN_MODE	0x003E

#define OCF_SET_AFH_CLASSIFICATION	0x003F
typedef struct {
	guint8		map[10];
} __attribute__ ((packed)) tFBS_set_afh_classification_cp;
#define SET_AFH_CLASSIFICATION_CP_SIZE 10
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_set_afh_classification_rp;
#define SET_AFH_CLASSIFICATION_RP_SIZE 1

#define OCF_READ_INQUIRY_SCAN_TYPE	0x0042
typedef struct {
	guint8		status;
	guint8		type;
} __attribute__ ((packed)) tFBS_read_inquiry_scan_type_rp;
#define READ_INQUIRY_SCAN_TYPE_RP_SIZE 2

#define OCF_WRITE_INQUIRY_SCAN_TYPE	0x0043
typedef struct {
	guint8		type;
} __attribute__ ((packed)) tFBS_write_inquiry_scan_type_cp;
#define WRITE_INQUIRY_SCAN_TYPE_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_inquiry_scan_type_rp;
#define WRITE_INQUIRY_SCAN_TYPE_RP_SIZE 1

#define OCF_READ_INQUIRY_MODE		0x0044
typedef struct {
	guint8		status;
	guint8		mode;
} __attribute__ ((packed)) tFBS_read_inquiry_mode_rp;
#define READ_INQUIRY_MODE_RP_SIZE 2

#define OCF_WRITE_INQUIRY_MODE		0x0045
typedef struct {
	guint8		mode;
} __attribute__ ((packed)) tFBS_write_inquiry_mode_cp;
#define WRITE_INQUIRY_MODE_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_inquiry_mode_rp;
#define WRITE_INQUIRY_MODE_RP_SIZE 1

#define OCF_READ_PAGE_SCAN_TYPE		0x0046

#define OCF_WRITE_PAGE_SCAN_TYPE	0x0047
	#define PAGE_SCAN_TYPE_STANDARD		0x00
	#define PAGE_SCAN_TYPE_INTERLACED	0x01

#define OCF_READ_AFH_MODE		0x0048
typedef struct {
	guint8		status;
	guint8		mode;
} __attribute__ ((packed)) tFBS_read_afh_mode_rp;
#define READ_AFH_MODE_RP_SIZE 2

#define OCF_WRITE_AFH_MODE		0x0049
typedef struct {
	guint8		mode;
} __attribute__ ((packed)) tFBS_write_afh_mode_cp;
#define WRITE_AFH_MODE_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_afh_mode_rp;
#define WRITE_AFH_MODE_RP_SIZE 1

#define HCI_MAX_EIR_LENGTH		240

#define OCF_READ_EXT_INQUIRY_RESPONSE	0x0051
typedef struct {
	guint8		status;
	guint8		fec;
	guint8		data[HCI_MAX_EIR_LENGTH];
} __attribute__ ((packed)) tFBS_read_ext_inquiry_response_rp;
#define READ_EXT_INQUIRY_RESPONSE_RP_SIZE 242

#define OCF_WRITE_EXT_INQUIRY_RESPONSE	0x0052
typedef struct {
	guint8		fec;
	guint8		data[HCI_MAX_EIR_LENGTH];
} __attribute__ ((packed)) tFBS_write_ext_inquiry_response_cp;
#define WRITE_EXT_INQUIRY_RESPONSE_CP_SIZE 241
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_ext_inquiry_response_rp;
#define WRITE_EXT_INQUIRY_RESPONSE_RP_SIZE 1

#define OCF_REFRESH_ENCRYPTION_KEY	0x0053
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_refresh_encryption_key_cp;
#define REFRESH_ENCRYPTION_KEY_CP_SIZE 2
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_refresh_encryption_key_rp;
#define REFRESH_ENCRYPTION_KEY_RP_SIZE 1

#define OCF_READ_SIMPLE_PAIRING_MODE	0x0055
typedef struct {
	guint8		status;
	guint8		mode;
} __attribute__ ((packed)) tFBS_read_simple_pairing_mode_rp;
#define READ_SIMPLE_PAIRING_MODE_RP_SIZE 2

#define OCF_WRITE_SIMPLE_PAIRING_MODE	0x0056
typedef struct {
	guint8		mode;
} __attribute__ ((packed)) tFBS_write_simple_pairing_mode_cp;
#define WRITE_SIMPLE_PAIRING_MODE_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_simple_pairing_mode_rp;
#define WRITE_SIMPLE_PAIRING_MODE_RP_SIZE 1

#define OCF_READ_LOCAL_OOB_DATA		0x0057
typedef struct {
	guint8		status;
	guint8		hash[16];
	guint8		randomizer[16];
} __attribute__ ((packed)) tFBS_read_local_oob_data_rp;
#define READ_LOCAL_OOB_DATA_RP_SIZE 33

#define OCF_READ_INQ_RESPONSE_TX_POWER_LEVEL	0x0058
typedef struct {
	guint8		status;
	int8_t		level;
} __attribute__ ((packed)) tFBS_read_inq_response_tx_power_level_rp;
#define READ_INQ_RESPONSE_TX_POWER_LEVEL_RP_SIZE 2

#define OCF_READ_INQUIRY_TRANSMIT_POWER_LEVEL	0x0058
typedef struct {
	guint8		status;
	int8_t		level;
} __attribute__ ((packed)) tFBS_read_inquiry_transmit_power_level_rp;
#define READ_INQUIRY_TRANSMIT_POWER_LEVEL_RP_SIZE 2

#define OCF_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL	0x0059
typedef struct {
	int8_t		level;
} __attribute__ ((packed)) tFBS_write_inquiry_transmit_power_level_cp;
#define WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_inquiry_transmit_power_level_rp;
#define WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_RP_SIZE 1

#define OCF_READ_DEFAULT_ERROR_DATA_REPORTING	0x005A
typedef struct {
	guint8		status;
	guint8		reporting;
} __attribute__ ((packed)) tFBS_read_default_error_data_reporting_rp;
#define READ_DEFAULT_ERROR_DATA_REPORTING_RP_SIZE 2

#define OCF_WRITE_DEFAULT_ERROR_DATA_REPORTING	0x005B
typedef struct {
	guint8		reporting;
} __attribute__ ((packed)) tFBS_write_default_error_data_reporting_cp;
#define WRITE_DEFAULT_ERROR_DATA_REPORTING_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_default_error_data_reporting_rp;
#define WRITE_DEFAULT_ERROR_DATA_REPORTING_RP_SIZE 1

#define OCF_ENHANCED_FLUSH		0x005F
typedef struct {
	guint16	handle;
	guint8		type;
} __attribute__ ((packed)) tFBS_enhanced_flush_cp;
#define ENHANCED_FLUSH_CP_SIZE 3

#define OCF_SEND_KEYPRESS_NOTIFY	0x0060
typedef struct {
	bdaddr_t	bdaddr;
	guint8		type;
} __attribute__ ((packed)) tFBS_send_keypress_notify_cp;
#define SEND_KEYPRESS_NOTIFY_CP_SIZE 7
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_send_keypress_notify_rp;
#define SEND_KEYPRESS_NOTIFY_RP_SIZE 1

#define OCF_READ_LOGICAL_LINK_ACCEPT_TIMEOUT	 0x0061
typedef struct {
	guint8		status;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_read_log_link_accept_timeout_rp;
#define READ_LOGICAL_LINK_ACCEPT_TIMEOUT_RP_SIZE 3

#define OCF_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT	0x0062
typedef struct {
	guint16	timeout;
} __attribute__ ((packed)) tFBS_write_log_link_accept_timeout_cp;
#define WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_CP_SIZE 2

#define OCF_SET_EVENT_MASK_PAGE_2	0x0063

#define OCF_READ_LOCATION_DATA		0x0064

#define OCF_WRITE_LOCATION_DATA	0x0065

#define OCF_READ_FLOW_CONTROL_MODE	0x0066

#define OCF_WRITE_FLOW_CONTROL_MODE	0x0067

#define OCF_READ_ENHANCED_TRANSMIT_POWER_LEVEL	0x0068
typedef struct {
	guint8		status;
	guint16	handle;
	int8_t		level_gfsk;
	int8_t		level_dqpsk;
	int8_t		level_8dpsk;
} __attribute__ ((packed)) tFBS_read_enhanced_transmit_power_level_rp;
#define READ_ENHANCED_TRANSMIT_POWER_LEVEL_RP_SIZE 6

#define OCF_READ_BEST_EFFORT_FLUSH_TIMEOUT	0x0069
typedef struct {
	guint8		status;
	guint32	timeout;
} __attribute__ ((packed)) tFBS_read_best_effort_flush_timeout_rp;
#define READ_BEST_EFFORT_FLUSH_TIMEOUT_RP_SIZE 5

#define OCF_WRITE_BEST_EFFORT_FLUSH_TIMEOUT	0x006A
typedef struct {
	guint16	handle;
	guint32	timeout;
} __attribute__ ((packed)) tFBS_write_best_effort_flush_timeout_cp;
#define WRITE_BEST_EFFORT_FLUSH_TIMEOUT_CP_SIZE 6
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_best_effort_flush_timeout_rp;
#define WRITE_BEST_EFFORT_FLUSH_TIMEOUT_RP_SIZE 1

#define OCF_READ_LE_HOST_SUPPORTED	0x006C
typedef struct {
	guint8		status;
	guint8		le;
	guint8		simul;
} __attribute__ ((packed)) tFBS_read_le_host_supported_rp;
#define READ_LE_HOST_SUPPORTED_RP_SIZE 3

#define OCF_WRITE_LE_HOST_SUPPORTED	0x006D
typedef struct {
	guint8		le;
	guint8		simul;
} __attribute__ ((packed)) tFBS_write_le_host_supported_cp;
#define WRITE_LE_HOST_SUPPORTED_CP_SIZE 2

/* Informational Parameters */
#define OGF_INFO_PARAM		0x04

#define OCF_READ_LOCAL_VERSION		0x0001
typedef struct {
	guint8		status;
	guint8		hci_ver;
	guint16	hci_rev;
	guint8		lmp_ver;
	guint16	manufacturer;
	guint16	lmp_subver;
} __attribute__ ((packed)) tFBS_read_local_version_rp;
#define READ_LOCAL_VERSION_RP_SIZE 9

#define OCF_READ_LOCAL_COMMANDS		0x0002
typedef struct {
	guint8		status;
	guint8		commands[64];
} __attribute__ ((packed)) tFBS_read_local_commands_rp;
#define READ_LOCAL_COMMANDS_RP_SIZE 65

#define OCF_READ_LOCAL_FEATURES		0x0003
typedef struct {
	guint8		status;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_read_local_features_rp;
#define READ_LOCAL_FEATURES_RP_SIZE 9

#define OCF_READ_LOCAL_EXT_FEATURES	0x0004
typedef struct {
	guint8		page_num;
} __attribute__ ((packed)) tFBS_read_local_ext_features_cp;
#define READ_LOCAL_EXT_FEATURES_CP_SIZE 1
typedef struct {
	guint8		status;
	guint8		page_num;
	guint8		max_page_num;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_read_local_ext_features_rp;
#define READ_LOCAL_EXT_FEATURES_RP_SIZE 11

#define OCF_READ_BUFFER_SIZE		0x0005
typedef struct {
	guint8		status;
	guint16	acl_mtu;
	guint8		sco_mtu;
	guint16	acl_max_pkt;
	guint16	sco_max_pkt;
} __attribute__ ((packed)) tFBS_read_buffer_size_rp;
#define READ_BUFFER_SIZE_RP_SIZE 8

#define OCF_READ_COUNTRY_CODE		0x0007

#define OCF_READ_BD_ADDR		0x0009
typedef struct {
	guint8		status;
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_read_bd_addr_rp;
#define READ_BD_ADDR_RP_SIZE 7

#define OCF_READ_DATA_BLOCK_SIZE	0x000A
typedef struct {
	guint8		status;
	guint16	max_acl_len;
	guint16	data_block_len;
	guint16	num_blocks;
} __attribute__ ((packed)) tFBS_read_data_block_size_rp;

/* Status params */
#define OGF_STATUS_PARAM	0x05

#define OCF_READ_FAILED_CONTACT_COUNTER		0x0001
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		counter;
} __attribute__ ((packed)) tFBS_read_failed_contact_counter_rp;
#define READ_FAILED_CONTACT_COUNTER_RP_SIZE 4

#define OCF_RESET_FAILED_CONTACT_COUNTER	0x0002
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_reset_failed_contact_counter_rp;
#define RESET_FAILED_CONTACT_COUNTER_RP_SIZE 3

#define OCF_READ_LINK_QUALITY		0x0003
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		link_quality;
} __attribute__ ((packed)) tFBS_read_link_quality_rp;
#define READ_LINK_QUALITY_RP_SIZE 4

#define OCF_READ_RSSI			0x0005
typedef struct {
	guint8		status;
	guint16	handle;
	int8_t		rssi;
} __attribute__ ((packed)) tFBS_read_rssi_rp;
#define READ_RSSI_RP_SIZE 4

#define OCF_READ_AFH_MAP		0x0006
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		mode;
	guint8		map[10];
} __attribute__ ((packed)) tFBS_read_afh_map_rp;
#define READ_AFH_MAP_RP_SIZE 14

#define OCF_READ_CLOCK			0x0007
typedef struct {
	guint16	handle;
	guint8		which_clock;
} __attribute__ ((packed)) tFBS_read_clock_cp;
#define READ_CLOCK_CP_SIZE 3
typedef struct {
	guint8		status;
	guint16	handle;
	guint32	clock;
	guint16	accuracy;
} __attribute__ ((packed)) tFBS_read_clock_rp;
#define READ_CLOCK_RP_SIZE 9

#define OCF_READ_LOCAL_AMP_INFO	0x0009
typedef struct {
	guint8		status;
	guint8		amp_status;
	guint32	total_bandwidth;
	guint32	max_guaranteed_bandwidth;
	guint32	min_latency;
	guint32	max_pdu_size;
	guint8		controller_type;
	guint16	pal_caps;
	guint16	max_amp_assoc_length;
	guint32	max_flush_timeout;
	guint32	best_effort_flush_timeout;
} __attribute__ ((packed)) tFBS_read_local_amp_info_rp;
#define READ_LOCAL_AMP_INFO_RP_SIZE 31

#define OCF_READ_LOCAL_AMP_ASSOC	0x000A
typedef struct {
	guint8		handle;
	guint16	length_so_far;
	guint16	assoc_length;
} __attribute__ ((packed)) tFBS_read_local_amp_assoc_cp;
#define READ_LOCAL_AMP_ASSOC_CP_SIZE 5
typedef struct {
	guint8		status;
	guint8		handle;
	guint16	length;
	guint8		fragment[HCI_MAX_NAME_LENGTH];
} __attribute__ ((packed)) tFBS_read_local_amp_assoc_rp;
#define READ_LOCAL_AMP_ASSOC_RP_SIZE 252

#define OCF_WRITE_REMOTE_AMP_ASSOC	0x000B
typedef struct {
	guint8		handle;
	guint16	length_so_far;
	guint16	remaining_length;
	guint8		fragment[HCI_MAX_NAME_LENGTH];
} __attribute__ ((packed)) tFBS_write_remote_amp_assoc_cp;
#define WRITE_REMOTE_AMP_ASSOC_CP_SIZE 253
typedef struct {
	guint8		status;
	guint8		handle;
} __attribute__ ((packed)) tFBS_write_remote_amp_assoc_rp;
#define WRITE_REMOTE_AMP_ASSOC_RP_SIZE 2

/* Testing commands */
#define OGF_TESTING_CMD		0x3e

#define OCF_READ_LOOPBACK_MODE			0x0001

#define OCF_WRITE_LOOPBACK_MODE			0x0002

#define OCF_ENABLE_DEVICE_UNDER_TEST_MODE	0x0003

#define OCF_WRITE_SIMPLE_PAIRING_DEBUG_MODE	0x0004
typedef struct {
	guint8		mode;
} __attribute__ ((packed)) tFBS_write_simple_pairing_debug_mode_cp;
#define WRITE_SIMPLE_PAIRING_DEBUG_MODE_CP_SIZE 1
typedef struct {
	guint8		status;
} __attribute__ ((packed)) tFBS_write_simple_pairing_debug_mode_rp;
#define WRITE_SIMPLE_PAIRING_DEBUG_MODE_RP_SIZE 1

/* LE commands */
#define OGF_LE_CTL		0x08

#define OCF_LE_SET_EVENT_MASK			0x0001
typedef struct {
	guint8		mask[8];
} __attribute__ ((packed)) tFBS_le_set_event_mask_cp;
#define LE_SET_EVENT_MASK_CP_SIZE 8

#define OCF_LE_READ_BUFFER_SIZE			0x0002
typedef struct {
	guint8		status;
	guint16	pkt_len;
	guint8		max_pkt;
} __attribute__ ((packed)) tFBS_le_read_buffer_size_rp;
#define LE_READ_BUFFER_SIZE_RP_SIZE 4

#define OCF_LE_READ_LOCAL_SUPPORTED_FEATURES	0x0003
typedef struct {
	guint8		status;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_le_read_local_supported_features_rp;
#define LE_READ_LOCAL_SUPPORTED_FEATURES_RP_SIZE 9

#define OCF_LE_SET_RANDOM_ADDRESS		0x0005
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_le_set_random_address_cp;
#define LE_SET_RANDOM_ADDRESS_CP_SIZE 6

#define OCF_LE_SET_ADVERTISING_PARAMETERS	0x0006
typedef struct {
	guint16	min_interval;
	guint16	max_interval;
	guint8		advtype;
	guint8		own_bdaddr_type;
	guint8		direct_bdaddr_type;
	bdaddr_t	direct_bdaddr;
	guint8		chan_map;
	guint8		filter;
} __attribute__ ((packed)) tFBS_le_set_advertising_parameters_cp;
#define LE_SET_ADVERTISING_PARAMETERS_CP_SIZE 15

#define OCF_LE_READ_ADVERTISING_CHANNEL_TX_POWER	0x0007
typedef struct {
	guint8		status;
	int8_t		level;
} __attribute__ ((packed)) tFBS_le_read_advertising_channel_tx_power_rp;
#define LE_READ_ADVERTISING_CHANNEL_TX_POWER_RP_SIZE 2

#define OCF_LE_SET_ADVERTISING_DATA		0x0008
typedef struct {
	guint8		length;
	guint8		data[31];
} __attribute__ ((packed)) tFBS_le_set_advertising_data_cp;
#define LE_SET_ADVERTISING_DATA_CP_SIZE 32

#define OCF_LE_SET_SCAN_RESPONSE_DATA		0x0009
typedef struct {
	guint8		length;
	guint8		data[31];
} __attribute__ ((packed)) tFBS_le_set_scan_response_data_cp;
#define LE_SET_SCAN_RESPONSE_DATA_CP_SIZE 32

#define OCF_LE_SET_ADVERTISE_ENABLE		0x000A
typedef struct {
	guint8		enable;
} __attribute__ ((packed)) tFBS_le_set_advertise_enable_cp;
#define LE_SET_ADVERTISE_ENABLE_CP_SIZE 1

#define OCF_LE_SET_SCAN_PARAMETERS		0x000B
typedef struct {
	guint8		type;
	guint16	interval;
	guint16	window;
	guint8		own_bdaddr_type;
	guint8		filter;
} __attribute__ ((packed)) tFBS_le_set_scan_parameters_cp;
#define LE_SET_SCAN_PARAMETERS_CP_SIZE 7

#define OCF_LE_SET_SCAN_ENABLE			0x000C
typedef struct {
	guint8		enable;
	guint8		filter_dup;
} __attribute__ ((packed)) tFBS_le_set_scan_enable_cp;
#define LE_SET_SCAN_ENABLE_CP_SIZE 2

#define OCF_LE_CREATE_CONN			0x000D
typedef struct {
	guint16	interval;
	guint16	window;
	guint8		initiator_filter;
	guint8		peer_bdaddr_type;
	bdaddr_t	peer_bdaddr;
	guint8		own_bdaddr_type;
	guint16	min_interval;
	guint16	max_interval;
	guint16	latency;
	guint16	supervision_timeout;
	guint16	min_ce_length;
	guint16	max_ce_length;
} __attribute__ ((packed)) tFBS_le_create_connection_cp;
#define LE_CREATE_CONN_CP_SIZE 25

#define OCF_LE_CREATE_CONN_CANCEL		0x000E

#define OCF_LE_READ_WHITE_LIST_SIZE		0x000F
typedef struct {
	guint8		status;
	guint8		size;
} __attribute__ ((packed)) tFBS_le_read_white_list_size_rp;
#define LE_READ_WHITE_LIST_SIZE_RP_SIZE 2

#define OCF_LE_CLEAR_WHITE_LIST			0x0010

#define OCF_LE_ADD_DEVICE_TO_WHITE_LIST		0x0011
typedef struct {
	guint8		bdaddr_type;
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_le_add_device_to_white_list_cp;
#define LE_ADD_DEVICE_TO_WHITE_LIST_CP_SIZE 7

#define OCF_LE_REMOVE_DEVICE_FROM_WHITE_LIST	0x0012
typedef struct {
	guint8		bdaddr_type;
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_le_remove_device_from_white_list_cp;
#define LE_REMOVE_DEVICE_FROM_WHITE_LIST_CP_SIZE 7

#define OCF_LE_CONN_UPDATE			0x0013
typedef struct {
	guint16	handle;
	guint16	min_interval;
	guint16	max_interval;
	guint16	latency;
	guint16	supervision_timeout;
	guint16	min_ce_length;
	guint16	max_ce_length;
} __attribute__ ((packed)) tFBS_le_connection_update_cp;
#define LE_CONN_UPDATE_CP_SIZE 14

#define OCF_LE_SET_HOST_CHANNEL_CLASSIFICATION	0x0014
typedef struct {
	guint8		map[5];
} __attribute__ ((packed)) tFBS_le_set_host_channel_classification_cp;
#define LE_SET_HOST_CHANNEL_CLASSIFICATION_CP_SIZE 5

#define OCF_LE_READ_CHANNEL_MAP			0x0015
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_le_read_channel_map_cp;
#define LE_READ_CHANNEL_MAP_CP_SIZE 2
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		map[5];
} __attribute__ ((packed)) tFBS_le_read_channel_map_rp;
#define LE_READ_CHANNEL_MAP_RP_SIZE 8

#define OCF_LE_READ_REMOTE_USED_FEATURES	0x0016
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_le_read_remote_used_features_cp;
#define LE_READ_REMOTE_USED_FEATURES_CP_SIZE 2

#define OCF_LE_ENCRYPT				0x0017
typedef struct {
	guint8		key[16];
	guint8		plaintext[16];
} __attribute__ ((packed)) tFBS_le_encrypt_cp;
#define LE_ENCRYPT_CP_SIZE 32
typedef struct {
	guint8		status;
	guint8		data[16];
} __attribute__ ((packed)) tFBS_le_encrypt_rp;
#define LE_ENCRYPT_RP_SIZE 17

#define OCF_LE_RAND				0x0018
typedef struct {
	guint8		status;
	guint64	random;
} __attribute__ ((packed)) tFBS_le_rand_rp;
#define LE_RAND_RP_SIZE 9

#define OCF_LE_START_ENCRYPTION			0x0019
typedef struct {
	guint16	handle;
	guint64	random;
	guint16	diversifier;
	guint8		key[16];
} __attribute__ ((packed)) tFBS_le_start_encryption_cp;
#define LE_START_ENCRYPTION_CP_SIZE 28

#define OCF_LE_LTK_REPLY			0x001A
typedef struct {
	guint16	handle;
	guint8		key[16];
} __attribute__ ((packed)) tFBS_le_ltk_reply_cp;
#define LE_LTK_REPLY_CP_SIZE 18
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_le_ltk_reply_rp;
#define LE_LTK_REPLY_RP_SIZE 3

#define OCF_LE_LTK_NEG_REPLY			0x001B
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_le_ltk_neg_reply_cp;
#define LE_LTK_NEG_REPLY_CP_SIZE 2
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_le_ltk_neg_reply_rp;
#define LE_LTK_NEG_REPLY_RP_SIZE 3

#define OCF_LE_READ_SUPPORTED_STATES		0x001C
typedef struct {
	guint8		status;
	guint64	states;
} __attribute__ ((packed)) tFBS_le_read_supported_states_rp;
#define LE_READ_SUPPORTED_STATES_RP_SIZE 9

#define OCF_LE_RECEIVER_TEST			0x001D
typedef struct {
	guint8		frequency;
} __attribute__ ((packed)) tFBS_le_receiver_test_cp;
#define LE_RECEIVER_TEST_CP_SIZE 1

#define OCF_LE_TRANSMITTER_TEST			0x001E
typedef struct {
	guint8		frequency;
	guint8		length;
	guint8		payload;
} __attribute__ ((packed)) tFBS_le_transmitter_test_cp;
#define LE_TRANSMITTER_TEST_CP_SIZE 3

#define OCF_LE_TEST_END				0x001F
typedef struct {
	guint8		status;
	guint16	num_pkts;
} __attribute__ ((packed)) tFBS_le_test_end_rp;
#define LE_TEST_END_RP_SIZE 3

#define OCF_LE_ADD_DEVICE_TO_RESOLV_LIST	0x0027
typedef struct {
	guint8		bdaddr_type;
	bdaddr_t	bdaddr;
	guint8		peer_irk[16];
	guint8		local_irk[16];
} __attribute__ ((packed)) tFBS_le_add_device_to_resolv_list_cp;
#define LE_ADD_DEVICE_TO_RESOLV_LIST_CP_SIZE 39

#define OCF_LE_REMOVE_DEVICE_FROM_RESOLV_LIST	0x0028
typedef struct {
	guint8		bdaddr_type;
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_le_remove_device_from_resolv_list_cp;
#define LE_REMOVE_DEVICE_FROM_RESOLV_LIST_CP_SIZE 7

#define OCF_LE_CLEAR_RESOLV_LIST		0x0029

#define OCF_LE_READ_RESOLV_LIST_SIZE		0x002A
typedef struct {
	guint8		status;
	guint8		size;
} __attribute__ ((packed)) tFBS_le_read_resolv_list_size_rp;
#define LE_READ_RESOLV_LIST_SIZE_RP_SIZE 2

#define OCF_LE_SET_ADDRESS_RESOLUTION_ENABLE	0x002D
typedef struct {
	guint8		enable;
} __attribute__ ((packed)) tFBS_le_set_address_resolution_enable_cp;
#define LE_SET_ADDRESS_RESOLUTION_ENABLE_CP_SIZE 1

/* Vendor specific commands */
#define OGF_VENDOR_CMD		0x3f

/* ---- HCI Events ---- */

typedef struct {
	guint8		type;
	guint8		evt_code;
	guint8		evt_len;
	guint8		data[0];
} __attribute__ ((packed)) tFBS_evt_header;

#define EVT_INQUIRY_COMPLETE		0x01

#define EVT_INQUIRY_RESULT		0x02
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pscan_rep_mode;
	guint8		pscan_period_mode;
	guint8		pscan_mode;
	guint8		dev_class[3];
	guint16	clock_offset;
} __attribute__ ((packed)) tFBS_inquiry_info;
#define INQUIRY_INFO_SIZE 14

#define EVT_CONN_COMPLETE		0x03
typedef struct {
	guint8		status;
	guint16	handle;
	bdaddr_t	bdaddr;
	guint8		link_type;
	guint8		encr_mode;
} __attribute__ ((packed)) tFBS_evt_conn_complete;
#define EVT_CONN_COMPLETE_SIZE 11

#define EVT_CONN_REQUEST		0x04
typedef struct {
	bdaddr_t	bdaddr;
	guint8		dev_class[3];
	guint8		link_type;
} __attribute__ ((packed)) tFBS_evt_conn_request;
#define EVT_CONN_REQUEST_SIZE 10

#define EVT_DISCONN_COMPLETE		0x05
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		reason;
} __attribute__ ((packed)) tFBS_evt_disconn_complete;
#define EVT_DISCONN_COMPLETE_SIZE 4

#define EVT_AUTH_COMPLETE		0x06
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_evt_auth_complete;
#define EVT_AUTH_COMPLETE_SIZE 3

#define EVT_REMOTE_NAME_REQ_COMPLETE	0x07
typedef struct {
	guint8		status;
	bdaddr_t	bdaddr;
	guint8		name[HCI_MAX_NAME_LENGTH];
} __attribute__ ((packed)) tFBS_evt_remote_name_req_complete;
#define EVT_REMOTE_NAME_REQ_COMPLETE_SIZE 255

#define EVT_ENCRYPT_CHANGE		0x08
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		encrypt;
} __attribute__ ((packed)) tFBS_evt_encrypt_change;
#define EVT_ENCRYPT_CHANGE_SIZE 4

#define EVT_CHANGE_CONN_LINK_KEY_COMPLETE	0x09
typedef struct {
	guint8		status;
	guint16	handle;
}  __attribute__ ((packed)) tFBS_evt_change_conn_link_key_complete;
#define EVT_CHANGE_CONN_LINK_KEY_COMPLETE_SIZE 3

#define EVT_MASTER_LINK_KEY_COMPLETE		0x0A
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		key_flag;
} __attribute__ ((packed)) tFBS_evt_master_link_key_complete;
#define EVT_MASTER_LINK_KEY_COMPLETE_SIZE 4

#define EVT_READ_REMOTE_FEATURES_COMPLETE	0x0B
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_evt_read_remote_features_complete;
#define EVT_READ_REMOTE_FEATURES_COMPLETE_SIZE 11

#define EVT_READ_REMOTE_VERSION_COMPLETE	0x0C
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		lmp_ver;
	guint16	manufacturer;
	guint16	lmp_subver;
} __attribute__ ((packed)) tFBS_evt_read_remote_version_complete;
#define EVT_READ_REMOTE_VERSION_COMPLETE_SIZE 8

#define EVT_QOS_SETUP_COMPLETE		0x0D
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		flags;			/* Reserved */
	tFBS_hci_qos		qos;
} __attribute__ ((packed)) tFBS_evt_qos_setup_complete;
#define EVT_QOS_SETUP_COMPLETE_SIZE (4 + HCI_QOS_CP_SIZE)

#define EVT_CMD_COMPLETE		0x0E
typedef struct {
	guint8		ncmd;
	guint16	opcode;
	guint8 param[0];
} __attribute__ ((packed)) tFBS_evt_cmd_complete;
#define EVT_CMD_COMPLETE_SIZE 3

#define EVT_CMD_STATUS			0x0F
typedef struct {
	guint8		status;
	guint8		ncmd;
	guint16	opcode;
} __attribute__ ((packed)) tFBS_evt_cmd_status;
#define EVT_CMD_STATUS_SIZE 4

#define EVT_HARDWARE_ERROR		0x10
typedef struct {
	guint8		code;
} __attribute__ ((packed)) tFBS_evt_hardware_error;
#define EVT_HARDWARE_ERROR_SIZE 1

#define EVT_FLUSH_OCCURRED		0x11
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_evt_flush_occured;
#define EVT_FLUSH_OCCURRED_SIZE 2

#define EVT_ROLE_CHANGE			0x12
typedef struct {
	guint8		status;
	bdaddr_t	bdaddr;
	guint8		role;
} __attribute__ ((packed)) tFBS_evt_role_change;
#define EVT_ROLE_CHANGE_SIZE 8

#define EVT_NUM_COMP_PKTS		0x13
typedef struct {
	guint8		num_hndl;
	guint8		data[0]; /* variable length part */
} __attribute__ ((packed)) tFBS_evt_num_comp_pkts;
#define EVT_NUM_COMP_PKTS_SIZE 1

#define EVT_MODE_CHANGE			0x14
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		mode;
	guint16	interval;
} __attribute__ ((packed)) tFBS_evt_mode_change;
#define EVT_MODE_CHANGE_SIZE 6

#define EVT_RETURN_LINK_KEYS		0x15
typedef struct {
	guint8		num_keys;
	/* variable length part */
} __attribute__ ((packed)) tFBS_evt_return_link_keys;
#define EVT_RETURN_LINK_KEYS_SIZE 1

#define EVT_PIN_CODE_REQ		0x16
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_evt_pin_code_req;
#define EVT_PIN_CODE_REQ_SIZE 6

#define EVT_LINK_KEY_REQ		0x17
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_evt_link_key_req;
#define EVT_LINK_KEY_REQ_SIZE 6

#define EVT_LINK_KEY_NOTIFY		0x18
typedef struct {
	bdaddr_t	bdaddr;
	guint8		link_key[16];
	guint8		key_type;
} __attribute__ ((packed)) tFBS_evt_link_key_notify;
#define EVT_LINK_KEY_NOTIFY_SIZE 23

#define EVT_LOOPBACK_COMMAND		0x19

#define EVT_DATA_BUFFER_OVERFLOW	0x1A
typedef struct {
	guint8		link_type;
} __attribute__ ((packed)) tFBS_evt_data_buffer_overflow;
#define EVT_DATA_BUFFER_OVERFLOW_SIZE 1

#define EVT_MAX_SLOTS_CHANGE		0x1B
typedef struct {
	guint16	handle;
	guint8		max_slots;
} __attribute__ ((packed)) tFBS_evt_max_slots_change;
#define EVT_MAX_SLOTS_CHANGE_SIZE 3

#define EVT_READ_CLOCK_OFFSET_COMPLETE	0x1C
typedef struct {
	guint8		status;
	guint16	handle;
	guint16	clock_offset;
} __attribute__ ((packed)) tFBS_evt_read_clock_offset_complete;
#define EVT_READ_CLOCK_OFFSET_COMPLETE_SIZE 5

#define EVT_CONN_PTYPE_CHANGED		0x1D
typedef struct {
	guint8		status;
	guint16	handle;
	guint16	ptype;
} __attribute__ ((packed)) tFBS_evt_conn_ptype_changed;
#define EVT_CONN_PTYPE_CHANGED_SIZE 5

#define EVT_QOS_VIOLATION		0x1E
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_evt_qos_violation;
#define EVT_QOS_VIOLATION_SIZE 2

#define EVT_PSCAN_REP_MODE_CHANGE	0x20
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pscan_rep_mode;
} __attribute__ ((packed)) tFBS_evt_pscan_rep_mode_change;
#define EVT_PSCAN_REP_MODE_CHANGE_SIZE 7

#define EVT_FLOW_SPEC_COMPLETE		0x21
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		flags;
	guint8		direction;
	tFBS_hci_qos		qos;
} __attribute__ ((packed)) tFBS_evt_flow_spec_complete;
#define EVT_FLOW_SPEC_COMPLETE_SIZE (5 + HCI_QOS_CP_SIZE)

#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pscan_rep_mode;
	guint8		pscan_period_mode;
	guint8		dev_class[3];
	guint16	clock_offset;
	int8_t		rssi;
} __attribute__ ((packed)) tFBS_inquiry_info_with_rssi;
#define INQUIRY_INFO_WITH_RSSI_SIZE 14
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pscan_rep_mode;
	guint8		pscan_period_mode;
	guint8		pscan_mode;
	guint8		dev_class[3];
	guint16	clock_offset;
	int8_t		rssi;
} __attribute__ ((packed)) tFBS_inquiry_info_with_rssi_and_pscan_mode;
#define INQUIRY_INFO_WITH_RSSI_AND_PSCAN_MODE_SIZE 15

#define EVT_READ_REMOTE_EXT_FEATURES_COMPLETE	0x23
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		page_num;
	guint8		max_page_num;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_evt_read_remote_ext_features_complete;
#define EVT_READ_REMOTE_EXT_FEATURES_COMPLETE_SIZE 13

#define EVT_SYNC_CONN_COMPLETE		0x2C
typedef struct {
	guint8		status;
	guint16	handle;
	bdaddr_t	bdaddr;
	guint8		link_type;
	guint8		trans_interval;
	guint8		retrans_window;
	guint16	rx_pkt_len;
	guint16	tx_pkt_len;
	guint8		air_mode;
} __attribute__ ((packed)) tFBS_evt_sync_conn_complete;
#define EVT_SYNC_CONN_COMPLETE_SIZE 17

#define EVT_SYNC_CONN_CHANGED		0x2D
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		trans_interval;
	guint8		retrans_window;
	guint16	rx_pkt_len;
	guint16	tx_pkt_len;
} __attribute__ ((packed)) tFBS_evt_sync_conn_changed;
#define EVT_SYNC_CONN_CHANGED_SIZE 9

#define EVT_SNIFF_SUBRATING		0x2E
typedef struct {
	guint8		status;
	guint16	handle;
	guint16	max_tx_latency;
	guint16	max_rx_latency;
	guint16	min_remote_timeout;
	guint16	min_local_timeout;
} __attribute__ ((packed)) tFBS_evt_sniff_subrating;
#define EVT_SNIFF_SUBRATING_SIZE 11

#define EVT_EXTENDED_INQUIRY_RESULT	0x2F
typedef struct {
	bdaddr_t	bdaddr;
	guint8		pscan_rep_mode;
	guint8		pscan_period_mode;
	guint8		dev_class[3];
	guint16	clock_offset;
	int8_t		rssi;
	guint8		data[HCI_MAX_EIR_LENGTH];
} __attribute__ ((packed)) tFBS_extended_inquiry_info;
#define EXTENDED_INQUIRY_INFO_SIZE 254

#define EVT_ENCRYPTION_KEY_REFRESH_COMPLETE	0x30
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_evt_encryption_key_refresh_complete;
#define EVT_ENCRYPTION_KEY_REFRESH_COMPLETE_SIZE 3

#define EVT_IO_CAPABILITY_REQUEST	0x31
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_evt_io_capability_request;
#define EVT_IO_CAPABILITY_REQUEST_SIZE 6

#define EVT_IO_CAPABILITY_RESPONSE	0x32
typedef struct {
	bdaddr_t	bdaddr;
	guint8		capability;
	guint8		oob_data;
	guint8		authentication;
} __attribute__ ((packed)) tFBS_evt_io_capability_response;
#define EVT_IO_CAPABILITY_RESPONSE_SIZE 9

#define EVT_USER_CONFIRM_REQUEST	0x33
typedef struct {
	bdaddr_t	bdaddr;
	guint32	passkey;
} __attribute__ ((packed)) tFBS_evt_user_confirm_request;
#define EVT_USER_CONFIRM_REQUEST_SIZE 10

#define EVT_USER_PASSKEY_REQUEST	0x34
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_evt_user_passkey_request;
#define EVT_USER_PASSKEY_REQUEST_SIZE 6

#define EVT_REMOTE_OOB_DATA_REQUEST	0x35
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_evt_remote_oob_data_request;
#define EVT_REMOTE_OOB_DATA_REQUEST_SIZE 6

#define EVT_SIMPLE_PAIRING_COMPLETE	0x36
typedef struct {
	guint8		status;
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_evt_simple_pairing_complete;
#define EVT_SIMPLE_PAIRING_COMPLETE_SIZE 7

#define EVT_LINK_SUPERVISION_TIMEOUT_CHANGED	0x38
typedef struct {
	guint16	handle;
	guint16	timeout;
} __attribute__ ((packed)) tFBS_evt_link_supervision_timeout_changed;
#define EVT_LINK_SUPERVISION_TIMEOUT_CHANGED_SIZE 4

#define EVT_ENHANCED_FLUSH_COMPLETE	0x39
typedef struct {
	guint16	handle;
} __attribute__ ((packed)) tFBS_evt_enhanced_flush_complete;
#define EVT_ENHANCED_FLUSH_COMPLETE_SIZE 2

#define EVT_USER_PASSKEY_NOTIFY		0x3B
typedef struct {
	bdaddr_t	bdaddr;
	guint32	passkey;
} __attribute__ ((packed)) tFBS_evt_user_passkey_notify;
#define EVT_USER_PASSKEY_NOTIFY_SIZE 10

#define EVT_KEYPRESS_NOTIFY		0x3C
typedef struct {
	bdaddr_t	bdaddr;
	guint8		type;
} __attribute__ ((packed)) tFBS_evt_keypress_notify;
#define EVT_KEYPRESS_NOTIFY_SIZE 7

#define EVT_REMOTE_HOST_FEATURES_NOTIFY	0x3D
typedef struct {
	bdaddr_t	bdaddr;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_evt_remote_host_features_notify;
#define EVT_REMOTE_HOST_FEATURES_NOTIFY_SIZE 14

#define EVT_LE_META_EVENT	0x3E
typedef struct {
	guint8		subevent;
	guint8		data[0];
} __attribute__ ((packed)) tFBS_evt_le_meta_event;
#define EVT_LE_META_EVENT_SIZE 1

#define EVT_LE_CONN_COMPLETE	0x01
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		role;
	guint8		peer_bdaddr_type;
	bdaddr_t	peer_bdaddr;
	guint16	interval;
	guint16	latency;
	guint16	supervision_timeout;
	guint8		master_clock_accuracy;
} __attribute__ ((packed)) tFBS_evt_le_connection_complete;
#define EVT_LE_CONN_COMPLETE_SIZE 18

#define EVT_LE_ADVERTISING_REPORT	0x02
typedef struct {
	guint8		evt_type;
	guint8		bdaddr_type;
	bdaddr_t	bdaddr;
	guint8		length;
	guint8		data[0];
} __attribute__ ((packed)) tFBS_le_advertising_info;
#define LE_ADVERTISING_INFO_SIZE 9

#define EVT_LE_CONN_UPDATE_COMPLETE	0x03
typedef struct {
	guint8		status;
	guint16	handle;
	guint16	interval;
	guint16	latency;
	guint16	supervision_timeout;
} __attribute__ ((packed)) tFBS_evt_le_connection_update_complete;
#define EVT_LE_CONN_UPDATE_COMPLETE_SIZE 9

#define EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE	0x04
typedef struct {
	guint8		status;
	guint16	handle;
	guint8		features[8];
} __attribute__ ((packed)) tFBS_evt_le_read_remote_used_features_complete;
#define EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE_SIZE 11

#define EVT_LE_LTK_REQUEST	0x05
typedef struct {
	guint16	handle;
	guint64	random;
	guint16	diversifier;
} __attribute__ ((packed)) tFBS_evt_le_long_term_key_request;
#define EVT_LE_LTK_REQUEST_SIZE 12

#define EVT_PHYSICAL_LINK_COMPLETE		0x40
typedef struct {
	guint8		status;
	guint8		handle;
} __attribute__ ((packed)) tFBS_evt_physical_link_complete;
#define EVT_PHYSICAL_LINK_COMPLETE_SIZE 2

#define EVT_CHANNEL_SELECTED		0x41

#define EVT_DISCONNECT_PHYSICAL_LINK_COMPLETE	0x42
typedef struct {
	guint8		status;
	guint8		handle;
	guint8		reason;
} __attribute__ ((packed)) tFBS_evt_disconn_physical_link_complete;
#define EVT_DISCONNECT_PHYSICAL_LINK_COMPLETE_SIZE 3

#define EVT_PHYSICAL_LINK_LOSS_EARLY_WARNING	0x43
typedef struct {
	guint8		handle;
	guint8		reason;
} __attribute__ ((packed)) tFBS_evt_physical_link_loss_warning;
#define EVT_PHYSICAL_LINK_LOSS_WARNING_SIZE 2

#define EVT_PHYSICAL_LINK_RECOVERY		0x44
typedef struct {
	guint8		handle;
} __attribute__ ((packed)) tFBS_evt_physical_link_recovery;
#define EVT_PHYSICAL_LINK_RECOVERY_SIZE 1

#define EVT_LOGICAL_LINK_COMPLETE		0x45
typedef struct {
	guint8		status;
	guint16	log_handle;
	guint8		handle;
	guint8		tx_flow_id;
} __attribute__ ((packed)) tFBS_evt_logical_link_complete;
#define EVT_LOGICAL_LINK_COMPLETE_SIZE 5

#define EVT_DISCONNECT_LOGICAL_LINK_COMPLETE	0x46

#define EVT_FLOW_SPEC_MODIFY_COMPLETE		0x47
typedef struct {
	guint8		status;
	guint16	handle;
} __attribute__ ((packed)) tFBS_evt_flow_spec_modify_complete;
#define EVT_FLOW_SPEC_MODIFY_COMPLETE_SIZE 3

#define EVT_NUMBER_COMPLETED_BLOCKS		0x48
typedef struct {
	guint16		handle;
	guint16		num_cmplt_pkts;
	guint16		num_cmplt_blks;
} __attribute__ ((packed)) tFBS_cmplt_handle;
typedef struct {
	guint16		total_num_blocks;
	guint8			num_handles;
	tFBS_cmplt_handle handles[0];
}  __attribute__ ((packed)) tFBS_evt_num_completed_blocks;

#define EVT_AMP_STATUS_CHANGE			0x4D
typedef struct {
	guint8		status;
	guint8		amp_status;
} __attribute__ ((packed)) tFBS_evt_amp_status_change;
#define EVT_AMP_STATUS_CHANGE_SIZE 2

#define EVT_TESTING			0xFE

#define EVT_VENDOR			0xFF

/* Internal events generated by BlueZ stack */
#define EVT_STACK_INTERNAL		0xFD
typedef struct {
	guint16	type;
	guint8		data[0];
} __attribute__ ((packed)) tFBS_evt_stack_internal;
#define EVT_STACK_INTERNAL_SIZE 2

#define EVT_SI_DEVICE	0x01
typedef struct {
	guint16	event;
	guint16	dev_id;
} __attribute__ ((packed)) tFBS_evt_si_device;
#define EVT_SI_DEVICE_SIZE 4

/* --------  HCI Packet structures  -------- */
#define HCI_TYPE_LEN	1

typedef struct {
	guint16	opcode;		/* OCF & OGF */
	guint8		plen;
} __attribute__ ((packed))	hci_command_hdr;
#define HCI_COMMAND_HDR_SIZE	3

typedef struct {
	guint8		evt;
	guint8		plen;
} __attribute__ ((packed))	hci_event_hdr;
#define HCI_EVENT_HDR_SIZE	2

typedef struct {
	guint16	handle;		/* Handle & Flags(PB, BC) */
	guint16	dlen;
} __attribute__ ((packed))	hci_acl_hdr;
#define HCI_ACL_HDR_SIZE	4

typedef struct {
	guint16	handle;
	guint8		dlen;
} __attribute__ ((packed))	hci_sco_hdr;
#define HCI_SCO_HDR_SIZE	3

typedef struct {
	guint16	device;
	guint16	type;
	guint16	plen;
} __attribute__ ((packed))	hci_msg_hdr;
#define HCI_MSG_HDR_SIZE	6

/* Command opcode pack/unpack */
#define FBS_CMD_OPCODE_PACK(ogf, ocf)	(guint16)((ocf & 0x03ff)|(ogf << 10))
#define FBS_CMD_OPCODE_OGF(op)		(op >> 10)
#define FBS_CMD_OPCODE_OCF(op)		(op & 0x03ff)

#endif /* FBS_HCI_TYPE_H__ */
