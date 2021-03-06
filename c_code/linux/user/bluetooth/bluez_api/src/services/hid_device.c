/*
 * hid_device.c
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co., Ltd
 * Author: Feng Shang <feng.shang@ingenic.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#include "hid_code.h"
#define HCI_DEV_ID 0

const struct gatts_if *gatts;
static bt_uuid_t *u_report_map, *u_hid_report, *u_hid_info;
static struct {
	UINT16 status;
	bdaddr_t addr;
	bt_uuid_t desc_uuid;
	int mtu;
	char is_paired;
} device;
static pthread_t thread;
static void* notify_heartrate(void *pdata);

char hid_map[] = {
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
char hid_info[] = {0x01, 0x02, 0x00, 0x03};
static char report[] = {0x00, 0x01}; /* report ID & report type */

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
		memset(&device, 0, sizeof(device));
		le_set_advertise_enable(HCI_DEV_ID);
	}else{
		report[0] = 0;
		device.mtu = BT_ATT_DEFAULT_LE_MTU;
		device.is_paired = 0;
	}
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, u_report_map)){
		if(device.mtu-1 >= sizeof(hid_map)){
			*read_rsp_buf_len = sizeof(hid_map);
			memcpy((char*)read_rsp_buf, hid_map, sizeof(hid_map));
		}else{
			*read_rsp_buf_len = device.mtu-1;
			memcpy((char*)read_rsp_buf, hid_map, device.mtu-1);
		}
	}else if(!bt_uuid_cmp(chac_uuid, u_hid_info)){
		*read_rsp_buf_len = sizeof(hid_info);
		memcpy((char*)read_rsp_buf, hid_info, sizeof(hid_info));
	}else if(!bt_uuid_cmp(chac_uuid, u_hid_report)){
	}else{
	}
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
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
	int sec, cnt = 30;
	while(cnt--){
		sec = att_get_security(addr);
		if(sec == BT_SECURITY_HIGH){
			device.status = desc;
			device.addr = *addr;
			device.desc_uuid = *desc_uuid;
			device.is_paired = 1;
			return;
		}
		usleep(500000);
	}
	printf("HID device paired timeout\n");
}
static void onMtuChanged(bdaddr_t *addr, int mtu, void *pdata)
{
	device.mtu = mtu;
}

static void onCharacterReadBlob(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		int offset, UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, u_report_map)){
		if(device.mtu-1 >= sizeof(hid_map)-offset){
			*read_rsp_buf_len = sizeof(hid_map)-offset;
			memcpy((char*)read_rsp_buf, &hid_map[offset], sizeof(hid_map)-offset);
		}else{
			*read_rsp_buf_len = device.mtu-1;
			memcpy((char*)read_rsp_buf, &hid_map[offset], device.mtu-1);
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
	struct gatt_character *report_map, *hid_info, *hid_report1, *hid_report2;

	u_report_map = bt_create_uuid_from_string(UUID_GATT_REPORT_MAP_CHARAC);
	u_hid_report = bt_create_uuid_from_string(UUID_GATT_REPORT_CHARAC);
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
	hid_info = create_character(u_hid_info,
			BT_GATT_CHRC_PROP_READ |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY);
	service_add_character(service, report_map);
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

void send_ascII(char ch)
{
	char press[] = {0,0,0,0,0,0,0,0};
	char release[] = {0,0,0,0,0,0,0,0};
	if(device.is_paired == 1 &&
			gatts && (device.status & DESCREPTOR_NOTIFICATION)){
		if(ch > 128 || key_map[(unsigned char)ch].hid_code == 0){
			printf("key value %d not support\n", ch);
		}else{
			printf("press key -> 0x%02x\n", ch);
			press[0] = key_map[(unsigned char)ch].hid_modifier;
			press[2] = key_map[(unsigned char)ch].hid_code;
			gatts->sendNotification(&device.addr, &device.desc_uuid,
					(UINT8*)&press, sizeof(press));
			gatts->sendNotification(&device.addr, &device.desc_uuid,
					(UINT8*)&release, sizeof(release));
		}
	}else{
		printf("Hid not ready\n");
	}
}

static void* notify_heartrate(void *pdata)
{
	char ch;
	while(1){
		system("stty raw");
		ch = getchar();
		system("stty cooked");
		if(ch == 3){// ctrl + c
			exit(0);
		}
		send_ascII(ch);
	}
	return 0;
}
