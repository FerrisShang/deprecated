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
static bt_uuid_t *uuid_service, *uuid_alert_status, *uuid_ringer_set, *uuid_ringer_ctrl;
static struct {
	UINT16 status;
	bdaddr_t addr;
	bt_uuid_t desc_uuid;
} notify[2];
static UINT8 data_ringer_status;
static UINT8 data_ringer_set;
static UINT8 ringer_ctrl;

static pthread_t thread;
static void* my_thread(void *pdata);

static int getStatus(void)
{
	return 0x00;
}
static int getSet(void)
{
	return 0x00;
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
	if(!bt_uuid_cmp(chac_uuid, uuid_alert_status)){
		data_ringer_status = getStatus();
		*read_rsp_buf = (UINT8)data_ringer_status;
		*read_rsp_buf_len = 1;
	}else if(!bt_uuid_cmp(chac_uuid, uuid_ringer_set)){
		data_ringer_set = getSet();
		*read_rsp_buf = data_ringer_set;
		*read_rsp_buf_len = 1;
	}
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	char *ctrl_str[] = {"", "Silent Mode", "Mute Once", "Cancel Silent Mode"};
	if(!bt_uuid_cmp(chac_uuid, uuid_ringer_ctrl)){
		if(*buf == 0 || *buf > 3){
			*buf = 1;
		}
		ringer_ctrl = *buf;
		printf("Ringer set to %s\n", ctrl_str[ringer_ctrl]);
	}
}
static void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
}
static void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
	if(!bt_uuid_cmp(desc_uuid, uuid_alert_status)){
		notify[0].status = desc;
		notify[0].addr = *addr;
		notify[0].desc_uuid = *desc_uuid;
	}else if(!bt_uuid_cmp(desc_uuid, uuid_ringer_set)){
		notify[1].status = desc;
		notify[1].addr = *addr;
		notify[1].desc_uuid = *desc_uuid;
	}
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

void init_phone_alert(void)
{
	struct gatt_service *service;
	struct gatt_character *chac_alert_status, *chac_ringer_set, *chac_ringer_ctrl;
	uuid_service = bt_create_uuid_from_string(UUID_SERV_PHONE_ALERT_STATUS);
	uuid_alert_status = bt_create_uuid_from_string(UUID_CHAC_ALERT_STATUS);
	uuid_ringer_set = bt_create_uuid_from_string(UUID_CHAC_RINGER_SETTING);
	uuid_ringer_ctrl = bt_create_uuid_from_string(UUID_CHAC_RINGER_CONTROL_POINT);
	service = create_service(uuid_service);
	chac_alert_status = create_character(uuid_alert_status,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_NOTIFY);
	chac_ringer_set = create_character(uuid_ringer_set,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_NOTIFY);
	chac_ringer_ctrl = create_character(uuid_ringer_ctrl, BT_GATT_CHRC_PROP_WRITE);
	service_add_character(service, chac_alert_status);
	service_add_character(service, chac_ringer_set);
	service_add_character(service, chac_ringer_ctrl);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init phone alert server failed");
	}else{
		pthread_create(&thread, NULL, my_thread, NULL);
	}
}

static void* my_thread(void *pdata)
{
	while(1){
		usleep(10000000); // 10 s
		if(gatts && (notify[0].status & DESCREPTOR_NOTIFICATION)){
			gatts->sendNotification(&notify[0].addr, &notify[0].desc_uuid,
					(UINT8*)&data_ringer_status, sizeof(UINT8));
		}else if(gatts && (notify[1].status & DESCREPTOR_NOTIFICATION)){
			gatts->sendNotification(&notify[1].addr, &notify[1].desc_uuid,
					(UINT8*)&data_ringer_set, sizeof(UINT8));
		}
	}
	return 0;
}

