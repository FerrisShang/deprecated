#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct tcps* tcps_create(
		void(*connected_cb)(int sock, struct sockaddr_un *addr, void *pdata),
		void(*recv_cb)(int sock, struct sockaddr_un *addr,
			char *buf, int len, void *pdata),
		void(*disconnected_cb)(int sock, struct sockaddr_un *addr, void *pdata),
		void *pdata);

int tcps_send(int sock, char *buf, int len);
int tcps_close(int sock);

#endif /* __TCP_SERVER_H__ */
