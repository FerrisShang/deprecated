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

gboolean FBS_hci_le_evt_process(guint8 evt, guint8 *data, guint16 len)
{
	if(evt == EVT_LE_META_EVENT){
		tFBS_evt_le_meta_event *p = (tFBS_evt_le_meta_event*)data;
		evt = p->subevent;
		data += sizeof(tFBS_evt_le_meta_event);
		len -= sizeof(tFBS_evt_le_meta_event);
	}else{
		return FALSE;
	}
	switch(evt){
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
			return TRUE;
			} break;	
		case EVT_LE_ADVERTISING_REPORT : {
			guint8 num = *data;
			data++;
			if(num != 1){
				g_warning("Le Advertising Report Number != 1");
			}
			tFBS_le_advertising_info *adv = (tFBS_le_advertising_info*)data;
			g_info("BLE adv report:%s %02x:%02x:%02x:%02x:%02x:%02x",
					adv->bdaddr_type==0?"PUBLIC":"RANDOM",
					adv->bdaddr[0],adv->bdaddr[1],adv->bdaddr[2],
					adv->bdaddr[3],adv->bdaddr[4],adv->bdaddr[5]);
			return TRUE;
			} break;	
		default :
			g_warning("Unprocessed LE event message, type : 0x%02x", evt);
			break;
	}
	return FALSE;
}
