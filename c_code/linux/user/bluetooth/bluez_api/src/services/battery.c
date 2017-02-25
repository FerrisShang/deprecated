/*
 * battery.c
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
#include "ble_profile.h"
#define HCI_DEV_ID 0

const struct gatts_if *gatts;
static bt_uuid_t *uuid_service, *uuid_battery_level;
static struct {
	UINT16 status;
	bdaddr_t addr;
	bt_uuid_t desc_uuid;
} notify;
static pthread_t thread;
static void* my_thread(void *pdata);

static int getData(void)//battery level
{
	return (rand()%10)+90;
}

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
		memset(&notify, 0, sizeof(notify));
	}
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_battery_level)){
		*read_rsp_buf = getData();
		*read_rsp_buf_len = 1;
	}
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
}
static void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
}
static void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
	notify.status = desc;
	notify.addr = *addr;
	notify.desc_uuid = *desc_uuid;
}
static void onMtuChanged(bdaddr_t *addr, int mtu, void *pdata)
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

void init_battery(void)
{
	struct gatt_service *service;
	struct gatt_character *chac_battery_level;
	uuid_service = bt_create_uuid_from_string(UUID_SERV_BATTERY);
	uuid_battery_level = bt_create_uuid_from_string(UUID_CHAC_BATTERY_LEVEL);
	service = create_service(uuid_service);
	chac_battery_level = create_character(uuid_battery_level,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_NOTIFY);
	service_add_character(service, chac_battery_level);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init battery server failed");
	}else{
		pthread_create(&thread, NULL, my_thread, NULL);
	}
}

static void* my_thread(void *pdata)
{
	while(1){
		usleep(10000000); // 10 s
		if(gatts && (notify.status & DESCREPTOR_NOTIFICATION)){
			char bt_level = getData();
			gatts->sendNotification(&notify.addr, &notify.desc_uuid,
					(UINT8*)&bt_level, sizeof(bt_level));
		}
	}
	return 0;
}
