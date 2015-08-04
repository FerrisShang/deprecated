#ifndef __LINUX__
#include "TcpLink.h"
struct tcplink* InitLink(char *Ip, int port)
{
	struct tcplink* tcplink;
	tcplink = malloc(sizeof(struct tcplink));
	if(tcplink == NULL)return NULL;
	tcplink->sockVersion = MAKEWORD(2,2);
	if(WSAStartup(tcplink->sockVersion, &tcplink->data) != 0) return NULL;
	tcplink->serAddr.sin_family = AF_INET;
	tcplink->serAddr.sin_port = htons(port);
	tcplink->serAddr.sin_addr.S_un.S_addr = inet_addr(Ip);
	tcplink->tv_out.tv_sec = 0;
	tcplink->tv_out.tv_usec = 100;//means "ms" in windows NOT "us"
	/*
	   setsockopt(tcplink->sclient, SOL_SOCKET, SO_RCVTIMEO,(const char*)&tcplink->tv_out, sizeof(struct timeval));
	   if (connect(tcplink->sclient, (struct sockaddr *)&tcplink->serAddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR){
	   closesocket(tcplink->sclient);
	   return NULL;
	   }
	   */
	tcplink->recv_max = TCP_RECV_LEN;
	tcplink->recv_buf = malloc(tcplink->recv_max);
	if(tcplink->recv_buf == NULL){
		closesocket(tcplink->sclient);
		free(tcplink);
		return NULL;
	}
	return tcplink;
}

int tcp_comm(struct tcplink* tcplink, char *send_buf, int send_len,
		int(*func)(char *buf, int len))
{
	char *p;
	int ret,time_out = 0;
	tcplink->recv_len = 0;
	tcplink->recv_buf[0] = 0;
	p = tcplink->recv_buf;

	tcplink->sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcplink->sclient == INVALID_SOCKET)return -1;
	setsockopt(tcplink->sclient, SOL_SOCKET, SO_RCVTIMEO,(const char*)&tcplink->tv_out, sizeof(struct timeval));

	if (connect(tcplink->sclient, (struct sockaddr *)&tcplink->serAddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR){
		closesocket(tcplink->sclient);
		return -1;
	}

	send(tcplink->sclient, send_buf, send_len, 0);
	while(1){
		ret = recv(tcplink->sclient, p, tcplink->recv_max, 0);
		if(ret == 0){
			closesocket(tcplink->sclient);
			tcplink->recv_len = p - tcplink->recv_buf;
			*p = 0;
			return tcplink->recv_len;
		}else if(ret > 0){
			p += ret;
			tcplink->recv_len = p - tcplink->recv_buf;
			time_out = 0;
		}else{
			time_out++;
		}
		if(time_out > TCP_RECV_TIME_OUT_S*500/TCP_RECV_TIME_INTV_MS ||
				(func != NULL && func(tcplink->recv_buf, tcplink->recv_len)==1)){
			closesocket(tcplink->sclient);
			tcplink->recv_len = p - tcplink->recv_buf;
			*p = 0;
			return tcplink->recv_len;
		}
	}
}

void CloseLink(struct tcplink* tcplink)
{
	if(tcplink){
		closesocket(tcplink->sclient);
		free(tcplink->recv_buf);
		free(tcplink);
	}
	return;
}
#endif /* __LINUX__ */
