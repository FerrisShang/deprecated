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
static bt_uuid_t *uuid_service, *uuid_power_level;

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_power_level)){
		*read_rsp_buf_len = 1;
		read_rsp_buf[0] = (rand()%120)-100;
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

void init_tx_power(void)
{
	struct gatt_service *service;
	struct gatt_character *power_level;

	uuid_service = bt_create_uuid_from_string(UUID_SERV_TX_POWER);
	uuid_power_level = bt_create_uuid_from_string(UUID_CHAC_TX_POWER_LEVEL);

	service = create_service(uuid_service);
	power_level = create_character(uuid_power_level, BT_GATT_CHRC_PROP_READ);
	service_add_character(service, power_level);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init tx power server failed");
	}
}


