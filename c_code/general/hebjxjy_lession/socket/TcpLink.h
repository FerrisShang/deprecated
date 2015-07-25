#ifndef __LINUX__
#ifndef __TCPLINK_H__
#define __TCPLINK_H__

#include "windows.h"

#define TCP_RECV_LEN (1024*1024)
#define TCP_RECV_TIME_INTV_MS 100
#define TCP_RECV_TIME_OUT_S 16

struct tcplink{
	WORD sockVersion;
	WSADATA data;
	struct sockaddr_in serAddr;
	SOCKET sclient;
	struct timeval tv_out;
	char *recv_buf;
	int recv_len;
	int recv_max;
};


struct tcplink* InitLink(char *Ip, int port);
void CloseLink(struct tcplink* tcplink);
int tcp_comm(struct tcplink* tcplink, char *send_buf, int send_len, int(*func)(char *buf, int len));
/*
struct tcplink* InitLink(char *Ip, int port, int recvTimeout);
void CloseLink(struct tcplink* tcplink);
int SendData(struct tcplink* tcplink, char *data, int len);
int recvData(struct tcplink* tcplink, char *data, int len);
*/

#endif /* __TCPLINK_H__ */
#endif /* __LINUX__ */
