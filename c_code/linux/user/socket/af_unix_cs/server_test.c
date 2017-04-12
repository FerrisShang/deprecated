#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "af_server.h"

void connected_cb(int sock, struct sockaddr_in *addr, void *pdata)
{
	printf("%2d %s:%d connected\n",
			sock, inet_ntoa(addr->sin_addr), addr->sin_port);
}

void recv_cb(int sock, struct sockaddr_in *addr,
		char *buf, int len, void *pdata)
{
	printf("%2d %s:%d received:%d\n",
			sock, inet_ntoa(addr->sin_addr), addr->sin_port, len);
	tcps_send(sock, buf, len);
}

void disconnected_cb(int sock, struct sockaddr_in *addr, void *pdata)
{
	printf("%2d %s:%d disconnected\n",
			sock, inet_ntoa(addr->sin_addr),addr->sin_port);
}

int main(int argc, char *argv[])
{
	system("rm -f /tmp/exhibition.sock");
	tcps_create(connected_cb, recv_cb, disconnected_cb, NULL);
	while(1){
		sleep(~0l);
	}
}
