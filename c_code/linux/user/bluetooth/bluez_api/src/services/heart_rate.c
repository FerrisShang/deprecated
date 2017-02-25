/*
 * heart_rate.c
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
#define HCI_DEV_ID 0

const struct gatts_if *gatts;
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
void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
}
void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
}
void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
}
void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
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

void init_heart_rate(void)
{
	struct gatt_service *heart_service;
	struct gatt_character *measurement;
	heart_service = create_service(bt_create_uuid_from_string("180d"));
	measurement = create_character(bt_create_uuid_from_string("2a37"),
			BT_GATT_CHRC_PROP_NOTIFY);
	service_add_character(heart_service, measurement);
	gatts = register_gatt_service(heart_service, &io_cb, NULL);
	if(!gatts){
		Log.e("init heart rate server failed");
	}else{
		pthread_create(&thread, NULL, notify_heartrate, NULL);
	}
}

static int getHeartRate(void)
{
	return 80+rand()%10;
}

static void* notify_heartrate(void *pdata)
{
	while(1){
		usleep(2000000);
		if(gatts && (device.status & DESCREPTOR_NOTIFICATION)){
			static struct {
				char flag;
				unsigned char value;
			}heartRate_data;
			heartRate_data.flag = 0;
			heartRate_data.value = getHeartRate();
			gatts->sendNotification(&device.addr, &device.desc_uuid,
					(UINT8*)&heartRate_data, sizeof(heartRate_data));
		}
	}
	return 0;
}
