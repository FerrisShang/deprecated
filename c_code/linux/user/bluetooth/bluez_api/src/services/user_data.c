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
static bt_uuid_t *uuid_service, *uuid_first_name, *uuid_last_name, *uuid_email, *uuid_age;
static struct {
	char fname[80];
	char lname[80];
	char email[80];
	char age;
} data = {
	"Ingenic", "BlueZ", "bluez@ingenic.com", 20
};

static void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
	}
}
static void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_first_name)){
		*read_rsp_buf_len = strlen(data.fname);
		strcpy((char*)read_rsp_buf, data.fname);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_last_name)){
		*read_rsp_buf_len = strlen(data.lname);
		strcpy((char*)read_rsp_buf, data.lname);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_email)){
		*read_rsp_buf_len = strlen(data.email);
		strcpy((char*)read_rsp_buf, data.email);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_age)){
		*read_rsp_buf_len = 1;
		read_rsp_buf[0] = data.age;
	}
}
static void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	if(!bt_uuid_cmp(chac_uuid, uuid_first_name)){
		strcpy(data.fname, (char*)buf);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_last_name)){
		strcpy(data.lname, (char*)buf);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_email)){
		strcpy(data.email, (char*)buf);
	}else if(!bt_uuid_cmp(chac_uuid, uuid_age)){
		data.age = *buf;
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

void init_uesr_data(void)
{
	struct gatt_service *service;
	struct gatt_character *first_name, *last_name, *email, *age;

	uuid_service = bt_create_uuid_from_string(UUID_SERV_USER_DATA);
	uuid_first_name = bt_create_uuid_from_string(UUID_CHAC_FIRST_NAME);
	uuid_last_name = bt_create_uuid_from_string(UUID_CHAC_LAST_NAME);
	uuid_email = bt_create_uuid_from_string(UUID_CHAC_EMAIL_ADDRESS);
	uuid_age = bt_create_uuid_from_string(UUID_CHAC_AGE);

	service = create_service(uuid_service);
	first_name = create_character(uuid_first_name,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	last_name = create_character(uuid_last_name,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	email = create_character(uuid_email,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	age = create_character(uuid_age,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);

	service_add_character(service, first_name);
	service_add_character(service, last_name);
	service_add_character(service, email);
	service_add_character(service, age);
	gatts = register_gatt_service(service, &io_cb, NULL);
	if(!gatts){
		Log.e("init user data server failed");
	}
}

