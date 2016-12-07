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
static bt_uuid_t *uuid_service, *uuid_alert_level;

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	char *alert_str[] = {"No Alert", "Mild Alert", "High Alert"};
	if(buf[0] > 2){
		buf[0] = 0;
	}
	if(!bt_uuid_cmp(chac_uuid, uuid_alert_level)){
		printf("Alert level update : %s\n", alert_str[buf[0]]);
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

void init_immediate_alert(void)
{
	struct gatt_service *service;
	struct gatt_character *alert_level;

	uuid_service = bt_create_uuid_from_string(UUID_SERV_IMMEDIATE_ALERT);
	uuid_alert_level = bt_create_uuid_from_string(UUID_CHAC_ALERT_LEVEL);

	service = create_service(uuid_service);
	alert_level = create_character(uuid_alert_level, BT_GATT_CHRC_PROP_WRITE);

	service_add_character(service, alert_level);

	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init immediate alert server failed");
	}
}


