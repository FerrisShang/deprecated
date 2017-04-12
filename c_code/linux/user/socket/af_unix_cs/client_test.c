#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "af_client.h"

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
	tcpc = tcpc_create(recv_cb, close_cb, NULL);
	if(!tcpc){
		printf("failed\n");
	}
	tcpc_send(tcpc, "client send data\n", 18);
	sleep(1);
	tcpc_distory(tcpc);
	return 0;
}
