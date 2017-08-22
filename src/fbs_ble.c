#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "fbs_stack.h"
#include "fbs_ble.h"
#include "fbs_hci.h"
#include "fbs_l2cap.h"

struct fbs_hci_tcb {
	GMutex mutex;
	tFBS_le_hci_cb  evt_callback;
	tFBS_le_adv_report_cb report_callback;
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
			if(fbs_hci_tcb.report_callback){
				fbs_hci_tcb.report_callback(adv);
			}else{
				g_info("BLE adv report:%s %02x:%02x:%02x:%02x:%02x:%02x",
						adv->bdaddr_type==0?"PUBLIC":"RANDOM",
						adv->bdaddr[0],adv->bdaddr[1],adv->bdaddr[2],
						adv->bdaddr[3],adv->bdaddr[4],adv->bdaddr[5]);
			}
			return TRUE;
			} break;
		default :
			g_warning("Unprocessed LE event message, type : 0x%02x", evt);
			break;
	}
	return FALSE;
}

void FBS_le_set_adv_param(
		guint16	min_interval,
		guint16	max_interval,
		guint8		advtype,
		guint8		own_bdaddr_type,
		guint8		direct_bdaddr_type,
		bdaddr_t	direct_bdaddr,
		guint8		chan_map,
		guint8		filter)
{
	tFBS_le_set_advertising_parameters_cp cp;
	cp.min_interval       = min_interval;
	cp.max_interval       = max_interval;
	cp.advtype            = advtype;
	cp.own_bdaddr_type    = own_bdaddr_type;
	cp.direct_bdaddr_type = direct_bdaddr_type;
	memcpy(&cp.direct_bdaddr, &direct_bdaddr, sizeof(bdaddr_t));
	cp.chan_map           = chan_map;
	cp.filter             = filter;
	FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_ADVERTISING_PARAMETERS),
			(gpointer)&cp,
			sizeof(tFBS_le_set_advertising_parameters_cp));
}

void FBS_le_set_adv_data(guint8 len, guint8 *data)
{
	tFBS_le_set_advertising_data_cp cp;
	memset(&cp, 0, sizeof(tFBS_le_set_advertising_data_cp));
	cp.length = len;
	memcpy(cp.data, data, len);
	FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_ADVERTISING_DATA),
			(gpointer)&cp,
			sizeof(tFBS_le_set_advertising_data_cp));
}

void FBS_le_set_scan_resp_data(guint8 len, guint8 *data)
{
	tFBS_le_set_scan_response_data_cp cp;
	memset(&cp, 0, sizeof(tFBS_le_set_scan_response_data_cp));
	cp.length = len;
	memcpy(cp.data, data, len);
	FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_SCAN_RESPONSE_DATA),
			(gpointer)&cp,
			sizeof(tFBS_le_set_scan_response_data_cp));
}

void FBS_le_set_adv_enable(guint8 enable)
{
	tFBS_le_set_advertise_enable_cp cp;
	cp.enable = enable;
	FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_ADVERTISE_ENABLE),
			(gpointer)&cp,
			sizeof(tFBS_le_set_advertise_enable_cp));
}

void FBS_le_set_scan_param(
		guint8  type,
		guint16 interval,
		guint16 window,
		guint8  own_bdaddr_type,
		guint8  filter)
{
	tFBS_le_set_scan_parameters_cp cp;
	cp.type            = type;
	cp.interval        = interval;
	cp.window          = window;
	cp.own_bdaddr_type = own_bdaddr_type;
	cp.filter          = filter;
	FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_SCAN_PARAMETERS),
			(gpointer)&cp,
			sizeof(tFBS_le_set_scan_parameters_cp));
}

void FBS_le_set_scan_enable(guint8 enable, guint8 filter_dup)
{
	tFBS_le_set_scan_enable_cp cp;
	cp.enable          = enable;
	cp.filter_dup      = filter_dup;
	FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_SCAN_ENABLE),
			(gpointer)&cp,
			sizeof(tFBS_le_set_scan_enable_cp));
}

void FBS_le_set_report_callback(tFBS_le_adv_report_cb report_callback)
{
	if(fbs_hci_tcb.report_callback != NULL &&
			fbs_hci_tcb.report_callback != report_callback){
		g_warning("Le report callback overwrite");
	}
	fbs_hci_tcb.report_callback = report_callback;
}
