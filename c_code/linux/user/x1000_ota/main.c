#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "protocol.h"
#include "socket_if.h"

s8_t *remoteIP = "192.168.3.93";
int main(int argc, char *argv[])
{
	sProtocol_t *pro;
	int len;
	u8_t buf[512];

	pro = protocol_create(TYPE_ENCRYPT_NULL);
	len = pack_req_new_version(pro, buf, (u8_t*)"123456789abcdef", (u8_t*)"version_version");
	dbg_hex("pack_req_new_version hexdump", buf, len);
	while(1){
		s8_t buf[1024];
		s32_t len;
		sSocket_t *socket;
		socket = socket_create(remoteIP, 1000);
		socket_send(socket, buf,10);
		sleep(1);
		len = socket_recv(socket, buf,1024);
		if(len>0)
			printf("%s\n",buf);
		else
			socket_distory(socket);
	}
	exit(0);
}
