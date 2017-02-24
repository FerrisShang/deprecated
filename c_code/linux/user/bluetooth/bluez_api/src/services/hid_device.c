#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

const struct gatts_if *gatts;
static bt_uuid_t *u_report_map, *u_hid_report, *u_hid_ctrl_point, *u_hid_info;
static struct {
	UINT16 status;
	bdaddr_t addr;
	bt_uuid_t desc_uuid;
} device;
static pthread_t thread;
static void* notify_heartrate(void *pdata);

void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
		memset(&device, 0, sizeof(device));
		le_set_advertise_enable(HCI_DEV_ID);
	}
}
char r_map[] = {
	0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x85, 0x01,
	0x75, 0x01, 0x95, 0x08, 0x05, 0x07, 0x19, 0xe0,
	0x29, 0xe7, 0x15, 0x00, 0x25, 0x01, 0x81, 0x02,
	0x95, 0x01, 0x75, 0x08, 0x81, 0x03, 0x95, 0x05,
	0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x05,
	0x91, 0x02, 0x95, 0x01, 0x75, 0x03, 0x91, 0x03,
	0x95, 0x06, 0x75, 0x08, 0x15, 0x00, 0x26, 0xff,
	0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0xff, 0x81,
	0x00, 0xc0, 0x05, 0x0c, 0x09, 0x01, 0xa1, 0x01,
	0x85, 0x02, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01,
	0x95, 0x0d, 0x0a, 0x24, 0x02, 0x09, 0x40, 0x0a,
	0x23, 0x02, 0x0a, 0xae, 0x01, 0x0a, 0x21, 0x02,
	0x09, 0xb6, 0x09, 0xcd, 0x09, 0xb5, 0x09, 0xe2,
	0x09, 0xea, 0x09, 0xe9, 0x09, 0x30, 0x09, 0x40,
	0x81, 0x02, 0x95, 0x01, 0x75, 0x03, 0x81, 0x03,
	0xc0,                     
};
char r_info[] = {0x01, 0x02, 0x00, 0x03};
void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, u_report_map)){
		*read_rsp_buf_len = sizeof(r_map);
		memcpy((char*)read_rsp_buf, r_map, sizeof(r_map));
	}else if(!bt_uuid_cmp(chac_uuid, u_hid_info)){
		*read_rsp_buf_len = sizeof(r_info);
		memcpy((char*)read_rsp_buf, r_info, sizeof(r_info));
	}else if(!bt_uuid_cmp(chac_uuid, u_hid_report)){
	}else{
	}
}
void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	if(!bt_uuid_cmp(chac_uuid, u_hid_ctrl_point)){
		printf("u_hid_ctrl_point writing\n");
	}
}
void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
	if(!bt_uuid_cmp(desc_uuid, u_hid_report)){
		char report[] = {0x01, 0x01}; /* report ID & report type */
		memcpy((char*)ret_desc, report, sizeof(report));
	}
}
void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
	int delay = 30;
	while(delay--){
		if(BT_SECURITY_LOW != bt_att_get_security(gatts)){
			sleep(5);
			printf("pairing ok\n");
			break;
		}else{
			printf("waiting\n");
			sleep(1);
		}
	}
	device.status = desc;
	device.addr = *addr;
	device.desc_uuid = *desc_uuid;
}
void onMtuChanged(bdaddr_t *addr, int mtu, void *pdata)
{
}

static struct gatts_cb io_cb = {
	onConnectionStateChange,
	onCharacterRead,
	onCharacterWrite,
	onDescriptorRead,
	onDescriptorWrite,
	onMtuChanged,
};

#define UUID_GATT_HID_SERVICE          "1812"
#define UUID_GATT_REPORT_CHARAC        "2A4D"
#define UUID_GATT_REPORT_MAP_CHARAC    "2A4B"
#define UUID_GATT_HID_INFO_CHARAC      "2A4A"
#define UUID_GATT_HID_CTL_POINT_CHARAC "2A4C"

void init_hid_device(void)
{
	struct gatt_service *service;
	struct gatt_character *report_map, *hid_info, *hid_ctrl_point, *hid_report;

	u_report_map = bt_create_uuid_from_string(UUID_GATT_REPORT_MAP_CHARAC);
	u_hid_report = bt_create_uuid_from_string(UUID_GATT_REPORT_CHARAC);
	u_hid_ctrl_point = bt_create_uuid_from_string(UUID_GATT_HID_CTL_POINT_CHARAC);
	u_hid_info = bt_create_uuid_from_string(UUID_GATT_HID_INFO_CHARAC);

	service = create_service(bt_create_uuid_from_string(UUID_GATT_HID_SERVICE));
	report_map = create_character(u_report_map,
			BT_GATT_CHRC_PROP_READ |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY);
	hid_report = create_character(u_hid_report,
			BT_GATT_CHRC_PROP_READ |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY |
			BT_GATT_CHRC_PROP_REPORT_REF);
	hid_ctrl_point = create_character(u_hid_ctrl_point,
			BT_GATT_CHRC_PROP_WRITE);

	hid_info = create_character(u_hid_info,
			BT_GATT_CHRC_PROP_READ |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY);
	service_add_character(service, report_map);
	service_add_character(service, hid_ctrl_point);
	service_add_character(service, hid_info);
	service_add_character(service, hid_report);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init server failed");
	}else{
		pthread_create(&thread, NULL, notify_heartrate, NULL);
	}
}

static void* notify_heartrate(void *pdata)
{
	while(1){
		usleep(5000000);
		if(gatts && (device.status & DESCREPTOR_NOTIFICATION)){
			sleep(1);
			char data1[] = {0,0,9,0,0,0,0,0};
			char data2[] = {0,0,0,0,0,0,0,0};
			gatts->sendNotification(&device.addr, &device.desc_uuid,
					(UINT8*)&data1, sizeof(data1));
			gatts->sendNotification(&device.addr, &device.desc_uuid,
					(UINT8*)&data2, sizeof(data2));
		}
	}
	return 0;
}
