/*
 * generic_access.c
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
static bt_uuid_t *uuid_service, *uuid_device_name,
				 *uuid_apperance, *uuid_address, *uuid_conn_param;
static char device_name[] = "Ingenic BlueZ";
static unsigned short dev_apperance = 193;//   Watch: Sports
static char dev_address[] = {0x00,0x11,0x22,0x33,0x44,0x55,};
static unsigned short dev_conn_param[] = {6, 300, 10, 2000};

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_device_name)){
		*read_rsp_buf_len = sizeof(device_name);
		strcpy((char*)read_rsp_buf, device_name);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_apperance)){
		*read_rsp_buf_len = sizeof(unsigned short);
		*(unsigned short*)read_rsp_buf = dev_apperance;
	}else if(!bt_uuid_cmp(chac_uuid, uuid_address)){
		*read_rsp_buf_len = sizeof(dev_address);
		memcpy(read_rsp_buf, dev_address, sizeof(dev_address));
	}else if(!bt_uuid_cmp(chac_uuid, uuid_conn_param)){
		*read_rsp_buf_len = sizeof(dev_conn_param);
		memcpy(read_rsp_buf, dev_conn_param, sizeof(dev_conn_param));
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

void init_generic_access(void)
{
	struct gatt_service *service;
	struct gatt_character *name, *apperance, *address, *para_conn;

	uuid_service = bt_create_uuid_from_string(UUID_SERV_GENERIC_ACCESS);
	uuid_device_name = bt_create_uuid_from_string(UUID_CHAC_DEVICE_NAME);
	uuid_apperance = bt_create_uuid_from_string(UUID_CHAC_APPEARANCE);
	uuid_address = bt_create_uuid_from_string(UUID_CHAC_RECONNECTION_ADDRESS);
	uuid_conn_param = bt_create_uuid_from_string(UUID_CHAC_CONNECTION_PARAM);

	service = create_service(uuid_service);
	name = create_character(uuid_device_name, BT_GATT_CHRC_PROP_READ);
	apperance = create_character(uuid_apperance, BT_GATT_CHRC_PROP_READ);
	address = create_character(uuid_address, BT_GATT_CHRC_PROP_READ);
	para_conn = create_character(uuid_conn_param, BT_GATT_CHRC_PROP_READ);

	service_add_character(service, name);
	service_add_character(service, apperance);
	service_add_character(service, address);
	service_add_character(service, para_conn);

	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init generic access server failed");
	}
}


