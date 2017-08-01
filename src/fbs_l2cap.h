#ifndef FBS_L2CAP_H__
#define FBS_L2CAP_H__

#include "fbs_stack.h"

enum {
	FBS_L2CAP_LINK_BR_EDR = 0,
	FBS_L2CAP_LINK_BLE,
};

struct fbs_l2cap_edr_link_status {
	guint16 handle;
	//unfinished
};

struct fbs_l2cap_le_link_status {
	guint16 handle;
	guint8 role; // 0-master ; 1-slave
	guint8 peer_addr_type; // 0-public ; 1-random
	bdaddr_t peer_addr;
	guint16 conn_interval; // 0x0006~0x0c80, Time=N*1.25ms (7.5ms~4s)
	guint16 conn_latency; // 0~0x01f3
	guint16 supervision_tout; // 0x000a~0x0c80 Time=N*10ms (100ms~32s)
	guint8 master_clock_accuracy; // 0~0x7 -> 500ppm~20ppm
};

struct fbs_l2cap_link {
	guint8 type; // 0-BR/EDR ; 1-BLE
	union {
		struct fbs_l2cap_edr_link_status edr;
		struct fbs_l2cap_le_link_status ble;
	} link;
};

void FBS_l2cap_init(void);
void FBS_l2cap_send(guint16 handle, gpointer *data, gint len);
void FBS_l2cap_data_process(guchar *data, gint len);
void FBS_l2cap_comp_packets(guint16 handle, guint16 num);
void FBS_get_acl_mtu(guint16 *acl_mtu, guint16 *acl_max_pkt);
void FBS_get_sco_mtu(guint8 *sco_mtu, guint16 *sco_max_pkt);
void FBS_record_chip_buffer_size(guint16 acl_mtu, guint16 acl_max_pkt,
		guint8 sco_mtu, guint16 sco_max_pkt);

void FBS_l2cap_connected(struct fbs_l2cap_link *link);
void FBS_l2cap_disconnected(guint16 handle, guint8 reason);

#endif /* FBS_L2CAP_H__ */
