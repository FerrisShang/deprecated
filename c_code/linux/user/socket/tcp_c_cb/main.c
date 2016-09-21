#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "tcp_client.h"

#define GET_STR "GET /index.html HTTP/1.1\nHost: www.baidu.com\n\n"
#define HOST_NAME "www.baidu.com"

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
	char ip_addr[32];
	struct tcpc* tcpc;
	struct hostent *hp;

	hp = gethostbyname(HOST_NAME);
	if(hp && hp->h_addr_list){
		struct in_addr in;
		memcpy(&in.s_addr, *hp->h_addr_list, sizeof(in.s_addr));
		strcpy(ip_addr, inet_ntoa(in));
	}else{
		printf("parse domain name failed\n");
		return 0;
	}
	printf("IP:%s\n", ip_addr);
	tcpc = tcpc_create(ip_addr, PORT, recv_cb, close_cb, NULL);
	if(!tcpc){
		printf("failed\n");
	}
	tcpc_send(tcpc, GET_STR, sizeof(GET_STR)-1);
	sleep(1);
	tcpc_distory(tcpc);
	return 0;
}
