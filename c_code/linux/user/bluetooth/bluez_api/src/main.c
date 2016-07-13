#include <stdio.h>
#include <unistd.h>
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

const struct gatts_if *gatts;

void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
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
static UINT8 status = 0;
void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid, void *pdata,
		UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len)
{
	*read_rsp_buf_len = 1;
	read_rsp_buf[0] = status;
}
void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	status = buf[0];
	gatts->sendNotification(addr, desc_uuid, "hehehhe", 6);
}
void onMtuChanged(bdaddr_t *addr, int mtu, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}

int main(int argc, char *argv[])
{
	struct gatt_service *heart_rate;
	struct gatt_character* heart_rate_value;
	struct gatt_character* heart_rate_point;
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
	Log.v("start:%d", init_gatt(HCI_DEV_ID));
	heart_rate = create_service(bt_create_uuid_from_string(
				"12312312-1234-4321-3456-292929929292"));
	heart_rate_value = create_character(
			bt_create_uuid_from_string(
				"12312312-1234-4321-3456-292929929999"),
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE);
	heart_rate_point = create_character(
			bt_create_uuid_from_string(
				"12312312-1234-4321-3456-292929966666"),
			BT_GATT_CHRC_PROP_READ  |
			BT_GATT_CHRC_PROP_WRITE |
			BT_GATT_CHRC_PROP_NOTIFY);
	service_add_character(heart_rate, heart_rate_value);
	service_add_character(heart_rate, heart_rate_point);
	gatts = register_gatt_service(heart_rate, &io_cb, NULL);
	while(1){
		le_set_advertise_enable(HCI_DEV_ID);
		usleep(5000000);
		mem_dump();
	}
	return 0;
}
