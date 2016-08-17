#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

const struct gatts_if *gatts;

void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	if(GATT_STATUS_DISCONNECTED == newState){
		le_set_advertise_enable(HCI_DEV_ID);
	}else{
		//usleep(5000000);
		//le_disconnect(HCI_DEV_ID, addr);
	}

}
void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	int i;static int cnt;
	*read_rsp_buf_len = 8;
	for(i=0;i<8;i++)read_rsp_buf[i] = i+cnt++;
}
void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}
static struct {
	UINT16 status;
	bdaddr_t addr;
	bt_uuid_t desc_uuid;
} device;
void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid,
		void *pdata, UINT16 *ret_desc)
{
	*ret_desc = device.status;
}
void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT16 desc, void *pdata)
{
	device.status = desc;
	device.addr = *addr;
	device.desc_uuid = *desc_uuid;
}
void onMtuChanged(bdaddr_t *addr, int mtu, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}

int main(int argc, char *argv[])
{
	char adv_data[31]= {
		0x02, 0x01, 0x1a, 0x02, 0x0a, 0x0c, 0x03, 0x03,
		0xe7, 0xfe, 0x0a, 0x08, 0x4a, 0x7a, 0x41, 0x69,
		0x72, 0x73, 0x79, 0x6e, 0x63, 0x09, 0xff, 0x4a,
		0x5a, 0x78, 0x43, 0xb0, 0x12, 0x1f, 0xac,
	};
	struct gatt_service *wechat;
	struct gatt_character *wechat_read, *wechat_write, *wechat_indicate;
	struct gatts_cb io_cb = {
		onConnectionStateChange,
		onCharacterRead,
		onCharacterWrite,
		onDescriptorRead,
		onDescriptorWrite,
		onMtuChanged,
	};
	mgmt_setup(HCI_DEV_ID);
	le_set_random_address(HCI_DEV_ID);
	le_set_advertise_parameters(HCI_DEV_ID);
	init_gatt(HCI_DEV_ID);
	wechat = create_service(bt_create_uuid_from_string("FEE7"));
	wechat_read = create_character(bt_create_uuid_from_string("FEC7"),
			BT_GATT_CHRC_PROP_WRITE);
	wechat_write = create_character(bt_create_uuid_from_string("FEC8"),
			BT_GATT_CHRC_PROP_INDICATE);
	wechat_indicate = create_character(bt_create_uuid_from_string("FEC9"),
			BT_GATT_CHRC_PROP_READ);
	service_add_character(wechat, wechat_write);
	service_add_character(wechat, wechat_indicate);
	service_add_character(wechat, wechat_read);
	gatts = register_gatt_service(wechat, &io_cb, NULL);
	le_set_advertise_enable(HCI_DEV_ID);
	le_set_advertise_data(HCI_DEV_ID, adv_data);
	while(1){
		static UINT16 ret_num;
		ret_num++;
		usleep(1000000);
		if(device.status & DESCREPTOR_NOTIFICATION){
			UINT8 ret_data[2];
			sprintf((char*)ret_data, "%02d", ret_num%100);
			gatts->sendNotification(&device.addr, &device.desc_uuid,
					(UINT8*)ret_data, sizeof(ret_data));
		}
		mem_dump();
	}
	return 0;
}
