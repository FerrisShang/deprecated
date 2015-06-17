#include <string.h>
#include "chk_version.h"
int have_new_version(sOta_para_t *ota_para, s8_t *version_data)
{
	if(!strcmp(ota_para->uboot_version, &version_data[0])){
		return 0;
	}
	if(!strcmp(ota_para->kernel_version, &version_data[8])){
		return 0;
	}
	return 1;
}

s32_t check_version(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para)
{
	s8_t buf[1024];
	s32_t len, i;
	len = pack_req_new_version(pro, buf, ota_para->device_id, ota_para->uboot_version, ota_para->kernel_version);
	if(len <=0){
		return CHK_VERSION_ERROR;
	}
	if(socket_send(socket, buf, len)<0){
		return CHK_VERSION_ERROR;
	}
	for(i=0;i<CHK_VERSION_TIMEOUT_S*1000000/10000;i++){
		usleep(10000);
		len = socket_recv(socket, buf,1024);
		if(len>0){
			s8_t data[512];
			s32_t data_len;
			eOta_cmd_t cmd;
			if(PARSE_SUCCESS == parse_package(pro, buf, len, &cmd, data, &data_len)){
				if(cmd == CMD_ACK_NEW_VERSION && data_len > 0){
					//check version 
					if(have_new_version(ota_para, data)){
						dbg_print(DBG_INFO, CHK_VERSION_DBG, "check version success, have new version"
								".package URL:\n           %s\n", &data[16]);
						//save update version & url
						return CHK_VERSION_NO_NEW;
					}else{
						dbg_print(DBG_INFO, CHK_VERSION_DBG, "check version success, no new version.\n");
						return CHK_VERSION_HAVE_NEW;
					}
				}
			}
			dbg_print(DBG_WARNING, CHK_VERSION_DBG, "check version error.\n");
			return CHK_VERSION_ERROR;
		}
	}
	dbg_print(DBG_WARNING, CHK_VERSION_DBG, "receive data timeout.\n");
	return CHK_VERSION_ERROR;
}
