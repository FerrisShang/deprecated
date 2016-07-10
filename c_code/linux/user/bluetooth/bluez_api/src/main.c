#include <stdio.h>
#include <unistd.h>
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

void onConnectionStateChange(bdaddr_t *addr, int newState, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}
void onCharacterRead(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}
void onCharacterWrite(bdaddr_t *addr, bt_uuid_t *chac_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}
void onDescriptorRead(bdaddr_t *addr, bt_uuid_t *desc_uuid, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
}
void onDescriptorWrite(bdaddr_t *addr, bt_uuid_t *desc_uuid,
	  UINT8 *buf, UINT16 len, void *pdata)
{
	Log.v("%s@%d", __func__, __LINE__);
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
	struct gatt_service_cb io_cb = {
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
	heart_rate = create_service(bt_create_uuid_from_string(HEART_RATE_UUID));
	heart_rate_value = create_character(bt_create_uuid_from_string(HEART_RATE_MEASUREMENT_UUID), 0xff);
	heart_rate_point = create_character(bt_create_uuid_from_string(HEART_RATE_CONTROL_POINT_UUID), 0xff);
	service_add_character(heart_rate, heart_rate_value);
	service_add_character(heart_rate, heart_rate_point);
	register_gatt_service(heart_rate, &io_cb, NULL);
	while(1){
		le_set_advertise_enable(HCI_DEV_ID);
		usleep(5000000);
		mem_dump();
	}
	return 0;
}
