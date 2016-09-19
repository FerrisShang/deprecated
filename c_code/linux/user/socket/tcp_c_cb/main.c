#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tcp_client.h"

#define GET_STR "GET /index.html HTTP/1.1\nHost: www.baidu.com\n\n"
#define IP "220.181.112.244" //www.baidu.com

#define PORT 80

void recv_cb(char *buf, int len, void *pdata)
{
	int i;
	for(i=0;i<len;i++){
		printf("%c", buf[i]);
	}
}

void close_cb(void *pdata)
{
	printf("socket closed callback\n");
}

int main(int argc , char *argv[])
{
	struct tcpc* tcpc;
	tcpc = tcpc_create(IP, PORT, recv_cb, close_cb, NULL);
	if(!tcpc){
		printf("failed\n");
	}
	tcpc_send(tcpc, GET_STR, sizeof(GET_STR)-1);
	sleep(1);
	tcpc_distory(tcpc);
	return 0;
}
