#ifndef __LINUX__
#ifndef __TCPLINK_H__
#define __TCPLINK_H__

#include "windows.h"

struct tcplink{
	WORD sockVersion;  
	WSADATA data;   
	struct sockaddr_in serAddr;   
	SOCKET sclient; 
	struct timeval tv_out; 
};

struct tcplink* InitLink(char *Ip, int port, int recvTimeout);
void CloseLink(struct tcplink* tcplink);
int SendData(struct tcplink* tcplink, char *data, int len);
int recvData(struct tcplink* tcplink, char *data, int len);

#endif /* __TCPLINK_H__ */
#endif /* __LINUX__ */
