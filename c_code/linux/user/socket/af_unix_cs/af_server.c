#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include "af_server.h"

#define SERVER_PATH "/tmp/exhibition.sock"
struct tcps{
	int sock;
	struct sockaddr_un server;
	int client_num;
	pthread_t th_listen;
	void(*connected_cb)(int sock, struct sockaddr_in *addr, void *pdata);
	void(*recv_cb)(int sock, struct sockaddr_in *addr, char *buf, int len, void *pdata);
	void(*disconnected_cb)(int sock, struct sockaddr_in *addr, void *pdata);
	void *pdata;
};

struct tcpc {
	struct tcps *tcps;
	int sock;
	struct sockaddr_in addr;
};

static void *listen_thd(void *pdata);
static void *recv_thd(void *pdata);

struct tcps* tcps_create(
		void(*connected_cb)(int sock, struct sockaddr_in *addr, void *pdata),
		void(*recv_cb)(int sock, struct sockaddr_in *addr,
			char *buf, int len, void *pdata),
		void(*disconnected_cb)(int sock, struct sockaddr_in *addr, void *pdata),
		void *pdata)
{
	struct tcps *tcps;
	int res;
	if(!connected_cb || !recv_cb || !disconnected_cb){
		return NULL;
	}
	tcps = calloc(1, sizeof(struct tcps));
	if(!tcps){
		return NULL;
	}
	tcps->connected_cb = connected_cb;
	tcps->recv_cb = recv_cb;
	tcps->disconnected_cb = disconnected_cb;

	bzero(&tcps->server,sizeof(struct sockaddr_un));
	tcps->server.sun_family = AF_UNIX;
	strcpy(tcps->server.sun_path, SERVER_PATH);

	tcps->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (tcps->sock < 0) {
		printf("Could not create socket\n");
		goto create_socket_err;
	}

	if (bind(tcps->sock, (struct sockaddr *) &tcps->server,
				sizeof(struct sockaddr_un))<0) {
		perror("bind failed. Error");
		goto bond_socket_err;
	}
	listen(tcps->sock, 1);

	res = pthread_create(&tcps->th_listen, NULL, listen_thd, tcps);
	if(res != 0){
		goto create_thread_err;
	}
	return tcps;
create_thread_err :
bond_socket_err :
	close(tcps->sock);
create_socket_err :
	free(tcps);
	return NULL;
}

static void *listen_thd(void *pdata)
{
	struct tcps *tcps = pdata;
	int res;
	while(1){
		struct tcpc *tcpc;
		static int size = sizeof(struct sockaddr_in);
		int client;
		struct sockaddr_in addr;
		pthread_t th_recv;
		client = accept(tcps->sock, (struct sockaddr *)&addr, (socklen_t*)&size);
		if(client < 0){
			continue;
		}
		tcpc = calloc(1, sizeof(struct tcpc));
		if(!tcpc){
			printf("calloc memory failed\n");
			shutdown(client, SHUT_RDWR);
			continue;
		}
		tcpc->tcps = tcps;
		tcpc->sock = client;
		memcpy(&tcpc->addr, &addr, size);
		tcps->connected_cb(client, &addr, tcps->pdata);
		res = pthread_create(&th_recv, NULL, recv_thd, tcpc);
		if(res < 0){
			printf("create thread failed\n");
			tcps->disconnected_cb(client, &addr, tcps->pdata);
			shutdown(client, SHUT_RDWR);
			free(tcpc);
			continue;
		}
	}
	printf("tcp server closed\n");
	return NULL;
}
static void *recv_thd(void *pdata)
{
#define MAX_BUF_SIZE 4096
	struct tcpc *tcpc = pdata;
	struct tcps *tcps = tcpc->tcps;
	char buf[MAX_BUF_SIZE];
	int len;
	while(1){
		len = recv(tcpc->sock, buf, MAX_BUF_SIZE, 0);
		if(len <= 0){//socket closed or error
			tcps->disconnected_cb(tcpc->sock, &tcpc->addr, tcps->pdata);
			close(tcpc->sock);
			free(tcpc);
			break;
		}else{
			tcps->recv_cb(tcpc->sock, &tcpc->addr, buf, len, tcps->pdata);
		}
	}
	return NULL;
}

int tcps_send(int sock, char *buf, int len)
{
	return send(sock, buf, len, 0);
}
int tcps_close(int sock)
{
	return shutdown(sock, SHUT_RDWR);
}
