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
	int mtu;
} device;
static pthread_t thread;
static void* notify_heartrate(void *pdata);

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
static char report[] = {0x00, 0x01}; /* report ID & report type */

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
		memset(&device, 0, sizeof(device));
		le_set_advertise_enable(HCI_DEV_ID);
	}else{
		report[0] = 0;
		device.mtu = BT_ATT_DEFAULT_LE_MTU;
	}
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	printf("mtu=%d\n", device.mtu);
	if(!bt_uuid_cmp(chac_uuid, u_report_map)){
		printf("%d\n", __LINE__);
		if(device.mtu-1 >= sizeof(r_map)){
			*read_rsp_buf_len = sizeof(r_map);
			printf("l=%d,%d\n", *read_rsp_buf_len, __LINE__);
			memcpy((char*)read_rsp_buf, r_map, sizeof(r_map));
		}else{
			*read_rsp_buf_len = device.mtu-1;
			printf("l=%d,%d\n", *read_rsp_buf_len, __LINE__);
			memcpy((char*)read_rsp_buf, r_map, device.mtu-1);
		}
	}else if(!bt_uuid_cmp(chac_uuid, u_hid_info)){
		*read_rsp_buf_len = sizeof(r_info);
		memcpy((char*)read_rsp_buf, r_info, sizeof(r_info));
	}else if(!bt_uuid_cmp(chac_uuid, u_hid_report)){
	}else{
	}
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	if(!bt_uuid_cmp(chac_uuid, u_hid_ctrl_point)){
		printf("u_hid_ctrl_point writing\n");
	}
}
static void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
	if(!bt_uuid_cmp(desc_uuid, u_hid_report)){
		report[0]++;
		memcpy((char*)ret_desc, report, sizeof(report));
	}
}
static void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
	sleep(2);
	printf("pairing ok\n");
	device.status = desc;
	device.addr = *addr;
	device.desc_uuid = *desc_uuid;
}
static void onMtuChanged(bdaddr_t *addr, int mtu, void *pdata)
{
	device.mtu = mtu;
}

static void onCharacterReadBlob(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		int offset, UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, u_report_map)){
		if(device.mtu-1 >= sizeof(r_map)-offset){
			*read_rsp_buf_len = sizeof(r_map)-offset;
			memcpy((char*)read_rsp_buf, &r_map[offset], sizeof(r_map)-offset);
		}else{
			*read_rsp_buf_len = device.mtu-1;
			memcpy((char*)read_rsp_buf, &r_map[offset], device.mtu-1);
		}
	}
}

static struct gatts_cb io_cb = {
	onConnectionStateChange,
	onCharacterRead,
	onCharacterWrite,
	onDescriptorRead,
	onDescriptorWrite,
	onMtuChanged,
	onCharacterReadBlob,
};

#define UUID_GATT_HID_SERVICE          "1812"
#define UUID_GATT_REPORT_CHARAC        "2A4D"
#define UUID_GATT_REPORT_MAP_CHARAC    "2A4B"
#define UUID_GATT_HID_INFO_CHARAC      "2A4A"
#define UUID_GATT_HID_CTL_POINT_CHARAC "2A4C"

void init_hid_device(void)
{
	struct gatt_service *service;
	struct gatt_character *report_map, *hid_info, *hid_ctrl_point, *hid_report1, *hid_report2;

	u_report_map = bt_create_uuid_from_string(UUID_GATT_REPORT_MAP_CHARAC);
	u_hid_report = bt_create_uuid_from_string(UUID_GATT_REPORT_CHARAC);
	u_hid_ctrl_point = bt_create_uuid_from_string(UUID_GATT_HID_CTL_POINT_CHARAC);
	u_hid_info = bt_create_uuid_from_string(UUID_GATT_HID_INFO_CHARAC);

	service = create_service(bt_create_uuid_from_string(UUID_GATT_HID_SERVICE));
	report_map = create_character(u_report_map,
			BT_GATT_CHRC_PROP_READ |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY);
	hid_report1 = create_character(u_hid_report,
			BT_GATT_CHRC_PROP_READ |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY |
			BT_GATT_CHRC_PROP_REPORT_REF);
	hid_report2 = create_character(u_hid_report,
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
	service_add_character(service, hid_report1);
	service_add_character(service, hid_report2);

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
		usleep(15000000);
		if(gatts && (device.status & DESCREPTOR_NOTIFICATION)){
			sleep(5);
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
