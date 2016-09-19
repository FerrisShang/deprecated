#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct tcpc;

struct tcpc* tcpc_create(char *ip, int port,
		void(*recv_cb)(char *buf, int len, void *pdata),
		void(*close_cb)(void *pdata), void *pdata);
void tcpc_distory(struct tcpc *tcpc);
int tcpc_send(struct tcpc *sock, char *buf, int len);

#endif /* __TCP_CLIENT_H__ */
