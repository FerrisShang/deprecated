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
static bt_uuid_t *uuid_service, *uuid_running_speed;
static struct {
	UINT16 status;
	bdaddr_t addr;
	bt_uuid_t desc_uuid;
} notify;
static pthread_t thread;
static void* my_thread(void *pdata);

static char getData(char *buf)//running speed
{
	unsigned short speed = (8+(rand()%5)) * 256;
	unsigned char cadence = 12+(rand()%5);
	char *p = buf;
	*p++ = 0x03; //Stride Length & Total Distance is present
	*p++ = speed & 0xFF;
	*p++ = (speed >> 7) & 0xFF;
	*p++ = cadence;
	return p-buf+2+4;
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

void init_running_speed(void)
{
	struct gatt_service *service;
	struct gatt_character *chac_rsc_measurement;
	uuid_service = bt_create_uuid_from_string(UUID_SERV_RUNNING_SPEED);
	uuid_running_speed = bt_create_uuid_from_string(UUID_CHAC_RSC_MEASUREMENT);
	service = create_service(uuid_service);
	chac_rsc_measurement = create_character(uuid_running_speed, BT_GATT_CHRC_PROP_NOTIFY);
	service_add_character(service, chac_rsc_measurement);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init running speed server failed");
	}else{
		pthread_create(&thread, NULL, my_thread, NULL);
	}
}

static void* my_thread(void *pdata)
{
	while(1){
		usleep(1000000); // 1 s
		if(gatts && (notify.status & DESCREPTOR_NOTIFICATION)){
			char buf[32]={0}, len;
			len = getData(buf);
			gatts->sendNotification(&notify.addr, &notify.desc_uuid, (UINT8*)buf, len);
		}
	}
	return 0;
}

