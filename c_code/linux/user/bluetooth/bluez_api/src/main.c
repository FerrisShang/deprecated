/*
 * main.c
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
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

#define BLE_NAME "HID over GATT"
struct adv_data{
	uint8_t flag[3];
	uint8_t uuid[8];
	uint8_t name_header[2];
	uint8_t name[19];
}adv_data = { // !! sizeof(adv_data) MUST less or equal 32
	.flag = {0x2, 0x1, 0x1a},
	.uuid = {0x7, 0x3, 0x12, 0x18, 0x0f, 0x18, 0x0a, 0x18,},
	.name_header = {sizeof(BLE_NAME), 0x09},
	.name = BLE_NAME,
};

void init_gatt_services(void);
int main(int argc, char *argv[])
{
	mgmt_setup(HCI_DEV_ID);
	mgmt_set_name(HCI_DEV_ID,"GATT Server", "");
	mgmt_set_iocap(HCI_DEV_ID, SMP_IO_NO_INPUT_OUTPUT);
	le_set_random_address(HCI_DEV_ID);
	le_set_advertise_parameters(HCI_DEV_ID);
	init_gatt(HCI_DEV_ID);
	init_gatt_services();
	le_set_advertise_enable(HCI_DEV_ID);
	le_set_advertise_data(0, (char*)&adv_data);
	while(1){
		usleep(1000000);
	}
	return 0;
}
