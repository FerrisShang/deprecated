#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "protocol.h"
#include "socket_if.h"

s8_t *remoteIP = "192.168.3.93";
s8_t *msg = "X1000 online.\n";
int main(int argc, char *argv[])
{
	sProtocol_t *pro;
	s8_t buf[1024];
	s32_t len;
	sSocket_t *socket;
	pro = protocol_create(TYPE_ENCRYPT_NULL);
	socket = socket_create(remoteIP, 1000);
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
	socket_distory(socket);
	exit(0);
}
