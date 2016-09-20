#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct tcps* tcps_create(int port,
		void(*connected_cb)(int sock, struct sockaddr_in *addr, void *pdata),
		void(*recv_cb)(int sock, struct sockaddr_in *addr,
			char *buf, int len, void *pdata),
		void(*disconnected_cb)(int sock, struct sockaddr_in *addr, void *pdata),
		void *pdata);

int tcps_send(int sock, char *buf, int len);

#endif /* __TCP_SERVER_H__ */
