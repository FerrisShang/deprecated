#ifndef FBS_BLE_H__
#define FBS_BLE_H__

#include "fbs_hci.h"

typedef void (*tFBS_le_hci_cb)(guint8 evt, guint8 *data, guint16 len);
typedef void (*tFBS_le_adv_report_cb)(tFBS_le_advertising_info *adv);
void FBS_ble_init(tFBS_le_hci_cb callback);
gboolean FBS_hci_le_evt_process(guint8 evt, guint8 *data, guint16 len);

void FBS_le_set_adv_param(
		guint16	min_interval,
		guint16	max_interval,
		guint8		advtype,
		guint8		own_bdaddr_type,
		guint8		direct_bdaddr_type,
		bdaddr_t	direct_bdaddr,
		guint8		chan_map,
		guint8		filter);
void FBS_le_set_adv_data(guint8 len, guint8 *data);
void FBS_le_set_scan_resp_data(guint8 len, guint8 *data);
void FBS_le_set_adv_enable(guint8 enable);
void FBS_le_set_scan_param(
		guint8  type,
		guint16 interval,
		guint16 window,
		guint8  own_bdaddr_type,
		guint8  filter);
void FBS_le_set_scan_enable(guint8 enable, guint8 filter_dup);
void FBS_le_set_report_callback(tFBS_le_adv_report_cb report_callback);
#endif /* FBS_BLE_H__ */
