#include "base.h"
#include "activity.h"

s32_t activity_device(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para)
{
	s8_t buf[1024];
	s32_t len, i;
	if(ota_para->act_state == ACTIVITY_STATE_NOT_ACT){
		len = pack_req_activition(pro, buf, ota_para->device_id, ota_para->wifi_addr, ota_para->bt_addr);
		if(len <=0){
			return ACTIVITY_ERROR;
		}
		if(socket_send(socket, buf, len)<0){
			return ACTIVITY_ERROR;
		}
		for(i=0;i<ACT_RECV_TIMEOUT_S*(1000000/10000);i++){
			usleep(10000);
			len = socket_recv(socket, buf,1024);
			if(len>0){
				s8_t data[512];
				s32_t data_len;
				eOta_cmd_t cmd;
				if(PARSE_SUCCESS == parse_package(pro, buf, len, &cmd, data, &data_len)){
					if(cmd == CMD_ACK_ACTIVITION && data_len == 1){
						if(data[0] == 0 || data[0] == 1){
							dbg_print(DBG_WARNING, ACTIVE_DBG, "activity_success.\n");
							return ACTIVITY_SUCCESS;
						}
					}
				}
				dbg_print(DBG_WARNING, ACTIVE_DBG, "activity error.\n");
				return ACTIVITY_ERROR;
			}
		}
		dbg_print(DBG_WARNING, ACTIVE_DBG, "receive data timeout.\n");
		return ACTIVITY_ERROR;
	}
	return ACTIVITY_SUCCESS;
}
