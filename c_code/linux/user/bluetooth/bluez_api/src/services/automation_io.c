/*
 * automation_io.c
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

const static struct gatts_if *gatts;
static bt_uuid_t *uuid_service, *uuid_digital, *uuid_analog, *uuid_aggregate;
static struct {
	UINT8 digital;
	UINT16 analog;
} data;

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
	}
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_digital)){
		*read_rsp_buf_len = 1;
		read_rsp_buf[0] = data.digital;
	}else if(!bt_uuid_cmp(chac_uuid, uuid_analog)){
		*read_rsp_buf_len = 2;
		read_rsp_buf[0] = data.analog&0xFF;
		read_rsp_buf[1] = (data.analog>>8)&0xFF;
	}else if(!bt_uuid_cmp(chac_uuid, uuid_aggregate)){
		*read_rsp_buf_len = 3;
		read_rsp_buf[0] = data.digital;
		read_rsp_buf[1] = data.analog&0xFF;
		read_rsp_buf[2] = (data.analog>>8)&0xFF;
	}
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_digital)){
		data.digital = buf[0];
	}else if(!bt_uuid_cmp(chac_uuid, uuid_analog)){
		data.analog = buf[0]+(buf[1]<<8);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_aggregate)){
		data.digital = buf[0];
		data.analog = buf[1]+(buf[2]<<8);
	}
}
static void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
}
static void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
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

void init_automation_io(void)
{
	struct gatt_service *service;
	struct gatt_character *digital, *analog, *aggregate;

	uuid_service = bt_create_uuid_from_string(UUID_SERV_AUTOMATION_IO);
	uuid_digital = bt_create_uuid_from_string(UUID_CHAC_DIGITAL);
	uuid_analog = bt_create_uuid_from_string(UUID_CHAC_ANALOG);
	uuid_aggregate = bt_create_uuid_from_string(UUID_CHAC_AGGREGATE);

	service = create_service(uuid_service);
	digital = create_character(uuid_digital,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	analog = create_character(uuid_analog,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	aggregate = create_character(uuid_aggregate,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	service_add_character(service, digital);
	service_add_character(service, analog);
	service_add_character(service, aggregate);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init automation io server failed");
	}
}
