#include "init_enc.h"

s32_t req_enc_conn(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para)
{
	s8_t buf[1024];
	s32_t len, i;
	len = pack_req_encrypt_comm(pro, buf, ota_para->device_id);
	if(len <=0){
		return INIT_ENC_ERROR;
	}
	if(socket_send(socket, buf, len)<0){
		return INIT_ENC_SUCCESS;
	}
	for(i=0;i<INIT_ENC_TIMEOUT_S*(1000000/10000);i++){
		usleep(10000);
		len = socket_recv(socket, buf,1024);
		if(len>0){
			s8_t data[512];
			s32_t data_len;
			eOta_cmd_t cmd;
			if(PARSE_SUCCESS == parse_package(pro, buf, len, &cmd, data, &data_len)){
				if(cmd == CMD_ACK_ENCRYPT_COMM && data_len > 0){
					dbg_print(DBG_INFO, INIT_ENC_DBG, "init enc.\n");
					//init aes key
					if(update_aes(&pro->enc, data, data_len) == 0){
						return INIT_ENC_SUCCESS;
					}else{
						return INIT_ENC_ERROR;
					}
				}
			}
			return INIT_ENC_ERROR;
		}
	}
	dbg_print(DBG_WARNING, INIT_ENC_DBG, "receive data timeout.\n");
	return INIT_ENC_ERROR;
}

s32_t req_new_aes_key(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para)
{
	s8_t buf[1024];
	s32_t len, i;
	len = pack_req_new_key(pro, buf, ota_para->device_id);
	if(len <=0){
		return INIT_ENC_ERROR;
	}
	if(socket_send(socket, buf, len)<0){
		return INIT_ENC_SUCCESS;
	}
	for(i=0;i<UPDATE_AES__TIMEOUT_S*(1000000/10000);i++){
		usleep(10000);
		len = socket_recv(socket, buf,1024);
		if(len>0){
			s8_t data[512];
			s32_t data_len;
			eOta_cmd_t cmd;
			if(PARSE_SUCCESS == parse_package(pro, buf, len, &cmd, data, &data_len)){
				if(cmd == CMD_ACK_NEW_KEY && data_len > 0){
					//update aes key
					dbg_print(DBG_INFO, INIT_ENC_DBG, "update aes key.\n");
					if(update_aes(&pro->enc, data, data_len) == 0){
						return INIT_ENC_SUCCESS;
					}else{
						return INIT_ENC_ERROR;
					}
				}
			}
			return INIT_ENC_ERROR;
		}
	}
	dbg_print(DBG_WARNING, INIT_ENC_DBG, "receive data timeout.\n");
	return INIT_ENC_ERROR;
}
