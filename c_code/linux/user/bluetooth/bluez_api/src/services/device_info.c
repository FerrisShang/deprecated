/*
 * device_info.c
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
static bt_uuid_t *uuid_service, *uuid_serial_num, *uuid_hardVer, *uuid_softVer, *uuid_firmVer;
static char HARD_VERSION[] = "V1.0.0";
static char SOFT_VERSION[] = "V2.0.1";
static char FIRM_VERSION[] = "V1.1.5";
static char SERIAL_NUM[] = "0x1234567890";

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_hardVer)){
		*read_rsp_buf_len = sizeof(HARD_VERSION);
		strcpy((char*)read_rsp_buf, HARD_VERSION);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_softVer)){
		*read_rsp_buf_len = sizeof(SOFT_VERSION);
		strcpy((char*)read_rsp_buf, SOFT_VERSION);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_serial_num)){
		*read_rsp_buf_len = sizeof(SERIAL_NUM);
		strcpy((char*)read_rsp_buf, SERIAL_NUM);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_firmVer)){
		*read_rsp_buf_len = sizeof(FIRM_VERSION);
		strcpy((char*)read_rsp_buf, FIRM_VERSION);
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

void init_device_info(void)
{
	struct gatt_service *service;
	struct gatt_character *softVer, *firmVer, *serialNum, *hardVer;

	uuid_service = bt_create_uuid_from_string(UUID_SERV_DEVICE_INFORMATION);
	uuid_serial_num = bt_create_uuid_from_string(UUID_CHAC_SERIAL_NUMBER);
	uuid_hardVer = bt_create_uuid_from_string(UUID_CHAC_HARDWARE_REVISION);
	uuid_softVer = bt_create_uuid_from_string(UUID_CHAC_SOFTWARE_REVISION);
	uuid_firmVer = bt_create_uuid_from_string(UUID_CHAC_FIRMWARE_REVISION);

	service = create_service(uuid_service);
	hardVer = create_character(uuid_hardVer, BT_GATT_CHRC_PROP_READ);
	softVer = create_character(uuid_softVer, BT_GATT_CHRC_PROP_READ);
	firmVer = create_character(uuid_firmVer, BT_GATT_CHRC_PROP_READ);
	serialNum = create_character(uuid_serial_num, BT_GATT_CHRC_PROP_READ);

	service_add_character(service, hardVer);
	service_add_character(service, softVer);
	service_add_character(service, firmVer);
	service_add_character(service, serialNum);

	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init device info server failed");
	}
}

