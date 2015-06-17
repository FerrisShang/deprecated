#include <string.h>
#include "ota_para.h"

s32_t read_activity_state(s8_t *state)
{
	return 0;
}

s32_t write_activity_state(s8_t *state)
{
	return 0;
}


s32_t read_wifi_addr(s8_t *addr)
{
	return 0;
}

s32_t read_bt_addr(s8_t *addr)
{
	return 0;
}

s32_t read_local_version(s8_t *version)
{
	return 0;
}

s32_t write_update_version(s8_t *version)
{
	return 0;
}

s32_t write_update_url(s8_t *url)
{
	return 0;
}

s32_t write_update_flag(s8_t *flag)
{
	return 0;
}

s32_t read_ota_para(sOta_para_t *para)
{
#if 0
#else //just for test
	memcpy(para->device_id, "deviceid", PARA_DEVICE_ID_LEN);
	memcpy(para->chip_id, "chip__id", PARA_CHIP_ID_LEN);
	memcpy(para->wifi_addr, "112233", PARA_WIFI_ADDR_LEN);
	memcpy(para->bt_addr, "554433", PARA_BT_ADDR_LEN);
	memcpy(para->uboot_version, "1.1.6", PARA_UBOOT_VERSION_LEN);
	memcpy(para->kernel_version, "3.10.14", PARA_KERNEL_VERSION_LEN);
	para->act_state = ACTIVITY_STATE_NOT_ACT;

#endif
	return 0;
}
