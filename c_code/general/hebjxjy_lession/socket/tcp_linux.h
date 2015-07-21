#ifdef __LINUX__
#ifndef __TCPLINUX_H__
#define __TCPLINUX_H__

#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_RECV_LEN (1024*1024)
#define TCP_RECV_TIME_INTV_MS 100
#define TCP_RECV_TIME_OUT_S 16

struct tcplink{
	int sockfd;
	struct sockaddr_in r_addr;
	char *recv_buf;
	int recv_len;
	int recv_max;
};

struct tcplink* InitLink(char *Ip, int port);
void CloseLink(struct tcplink* tcplink);
int tcp_comm(struct tcplink* tcplink, char *send_buf, int send_len, int(*func)(char *buf, int len));

#endif /* __TCPLINUX_H__ */
#endif /* __LINUX__ */
