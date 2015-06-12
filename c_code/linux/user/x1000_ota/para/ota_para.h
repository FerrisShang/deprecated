#ifndef __OTA_PARA_H__
#define __OTA_PARA_H__

#include "base.h"
#include "para_if.h"

typedef struct{
	s8_t device_id[PARA_DEVICE_ID_LEN];
	s8_t wifi_addr[PARA_WIFI_ADDR_LEN];
	s8_t bt_addr[PARA_BT_ADDR_LEN];
	s8_t version[PARA_VERSION_LEN];
	s8_t act_state;
}sOta_para_t;

s32_t read_activity_state(s8_t *state);
s32_t write_activity_state(s8_t *state);

s32_t read_wifi_addr(s8_t *addr);
s32_t read_bt_addr(s8_t *addr);

s32_t read_local_version(s8_t *version);
s32_t write_update_version(s8_t *version);
s32_t write_update_url(s8_t *url);
s32_t write_update_flag(s8_t *flag);

s32_t read_ota_para(sOta_para_t *para);

#define ACTIVITY_STATE_ACT      1
#define ACTIVITY_STATE_NOT_ACT  0

#endif /* __OTA_PARA_H__ */
