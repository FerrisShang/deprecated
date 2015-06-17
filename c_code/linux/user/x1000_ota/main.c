#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ota_para.h"
#include "base.h"
#include "protocol.h"
#include "socket_if.h"
#include "init_enc.h"
#include "activity.h"
#include "chk_version.h"

s8_t *remoteIP = "192.168.1.200";
s8_t *msg = "X1000 online.\n";
int main(int argc, char *argv[])
{
	s32_t res;
	sProtocol_t *pro;
	sSocket_t *socket;
	sOta_para_t ota_para;
	while(1){
		read_ota_para(&ota_para);
		pro = protocol_create(TYPE_ENCRYPT);
		if(pro == 0){
			printf("protocol create failed.\n");
			exit(1);
		}
		socket = socket_create(remoteIP, 6000);
		if(socket == 0){
			printf("socket create failed.\n");
			exit(1);
		}

		res = INIT_ENC_SUCCESS;
		if(pro->mode != TYPE_ENCRYPT_NULL){//init encrypt communication
			res = req_enc_conn(pro, socket, &ota_para);
			if(res == INIT_ENC_ERROR){
				printf("req enc conn failed.\n");
			}
		}
		if(!(pro->mode != TYPE_ENCRYPT_NULL && res != INIT_ENC_SUCCESS)){
			res = activity_device(pro, socket, &ota_para);
			if(res == ACTIVITY_SUCCESS){
				res = check_version(pro, socket, &ota_para);
				if(res == CHK_VERSION_HAVE_NEW){
					//save boot select & reboot
				}
			}
		}
		sleep(1);
		socket_distory(socket);
		protocol_destory(pro);
		//sleep(3600);
		sleep(10);
	}

	/*
	s8_t buf[1024];
	s32_t len;
	socket_send(socket, msg, strlen(msg));
	while(1){
		// server send : 58 00 00 0c 4a 5a 00 08 11 11 11 11 00 07 01 f8
		len = socket_recv(socket, buf,1024);
		if(len>0){
			s8_t data[512];
			s32_t data_len;
			eOta_cmd_t cmd;
			if(PARSE_SUCCESS == parse_package(pro, buf, len, &cmd, data, &data_len)){
				printf("cmd = %d\n", cmd);
				dbg_hex("data", data, data_len);
			}
			len = pack_req_activition(pro, buf, (s8_t*)"3434343434343434",
					(s8_t*)"0000000000000000", (s8_t*)"1.2.3.4.5.6.7.8.");
			dbg_hex("pack_req_new_version hexdump", buf, len);
			printf("send:%d\n",socket_send(socket, buf, len));
		}
		usleep(50000);
	}
	*/
	socket_distory(socket);
	exit(0);
}
