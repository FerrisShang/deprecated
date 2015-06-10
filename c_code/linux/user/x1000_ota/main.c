#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "protocol.h"
#include "socket_if.h"

int main(int argc, char *argv[])
{
	protocol_create();
	while(1){
		char buf[1024];
		int len;
		sSocket_t *socket;
		socket = socket_create("192.168.1.112",1000);
		socket_send(socket, buf,10);
		sleep(1);
		len = socket_recv(socket, buf,1024);
		if(len>0)
			printf("%s\n",buf);
		else
			printf("len=%d\n",len);
		socket_distory(socket);
	}
	exit(0);
}
