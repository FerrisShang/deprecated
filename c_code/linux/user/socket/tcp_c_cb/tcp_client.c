#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tcp_client.h"

struct tcpc {
	int sock;
	struct sockaddr_in server;
	pthread_t th_recv;
	void(*recv_cb)(char *buf, int len, void *pdata);
	void(*close_cb)(void *pdata);
	void *pdata;
};

static void *recv_thd(void *pdata);

void tcpc_distory(struct tcpc *tcpc)
{
	if(!tcpc){
		return;
	}
	shutdown(tcpc->sock, SHUT_RDWR);
	pthread_join(tcpc->th_recv, NULL);
	free(tcpc);
}

struct tcpc* tcpc_create(char *ip, int port,
		void(*recv_cb)(char *buf, int len, void *pdata),
		void(*close_cb)(void *pdata),
		void *pdata)
{
	int ret;
	struct tcpc *tcpc;
	if(!recv_cb || !close_cb){
		return NULL;
	}
	tcpc = calloc(1, sizeof(struct tcpc));
	if(!tcpc){
		return NULL;
	}
	//Create socket
	tcpc->sock = socket(AF_INET , SOCK_STREAM , 0);
	if (tcpc->sock == -1)
	{
		printf("Could not create socket\n");
		goto create_socket_err;
	}

	tcpc->server.sin_addr.s_addr = inet_addr(ip);
	tcpc->server.sin_family = AF_INET;
	tcpc->server.sin_port = htons( port );

	//Connect to remote server
	if (connect(tcpc->sock , (struct sockaddr *)&tcpc->server,
				sizeof(tcpc->server)) < 0)
	{
		perror("connect failed. Error");
		goto create_socket_err;
	}

	tcpc->recv_cb = recv_cb;
	tcpc->close_cb = close_cb;
	tcpc->pdata = pdata;

	ret = pthread_create(&tcpc->th_recv, NULL, recv_thd, tcpc);
	if(ret < 0){
		printf("create thread failed\n");
		goto create_thread_err;
	}
	return tcpc;
create_thread_err :
	close(tcpc->sock);
create_socket_err :
	free(tcpc);
	return NULL;
}
static void *recv_thd(void *pdata)  
{
#define MAX_BUF_SIZE 4096
	char buf[MAX_BUF_SIZE];
	int len;
	struct tcpc *tcpc = pdata;
	while(1){
		len = recv(tcpc->sock, buf, MAX_BUF_SIZE, 0);
		if(len <= 0){//socket closed or error
			close(tcpc->sock);
			printf("socket is shutdown\n");
			tcpc->close_cb(tcpc->pdata);
			break;
		}else{
			tcpc->recv_cb(buf, len, tcpc->pdata);
		}
	}
	return NULL;
}

int tcpc_send(struct tcpc *tcpc, char *buf, int len)
{
	if(!tcpc){
		printf("tcpc is NULL\n");
		return -1;
	}
	return send(tcpc->sock, buf, len, 0);
}
