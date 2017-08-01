#ifndef FBS_BLE_H__
#define FBS_BLE_H__

typedef void (*tFBS_le_hci_cb)(guint8 evt, guint8 *data, guint16 len);
void FBS_ble_init(tFBS_le_hci_cb callback);
gboolean FBS_hci_le_evt_process(guint8 evt, guint8 *data, guint16 len);

#endif /* FBS_BLE_H__ */
