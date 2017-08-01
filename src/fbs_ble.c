#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "fbs_ble.h"
#include "fbs_hci.h"
#include "fbs_l2cap.h"

struct fbs_hci_tcb {
	GMutex mutex;
	tFBS_le_hci_cb  evt_callback;
};

static struct fbs_hci_tcb fbs_hci_tcb;

void FBS_ble_init(tFBS_le_hci_cb callback)
{
	g_assert(callback != NULL);
	memset(&fbs_hci_tcb, 0, sizeof(struct fbs_hci_tcb));
	fbs_hci_tcb.evt_callback = callback;
}

void FBS_hci_le_evt_process(guchar subevent, guchar *data, gint len)
{
	switch(subevent){
		case EVT_LE_CONN_COMPLETE : {
			tFBS_evt_le_connection_complete *p = (tFBS_evt_le_connection_complete*)data;
			struct fbs_l2cap_link conn;
			conn.type = FBS_L2CAP_LINK_BLE;
			conn.link.ble.handle = p->handle;
			conn.link.ble.role = p->role;
			conn.link.ble.peer_addr_type = p->peer_bdaddr_type;
			memcpy(&conn.link.ble.peer_addr, p->peer_bdaddr, sizeof(bdaddr_t));
			conn.link.ble.conn_interval = p->interval;
			conn.link.ble.conn_latency = p->latency;
			conn.link.ble.supervision_tout = p->supervision_timeout;
			conn.link.ble.master_clock_accuracy = p->master_clock_accuracy;
			FBS_l2cap_connected(&conn);
			} break;	
		case EVT_LE_ADVERTISING_REPORT : {
			;
			} break;	
		default :
			break;
	}
	g_warning("Unprocessed LE event message, type : 0x%02x", subevent);
}
