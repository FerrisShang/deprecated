#ifndef __SOCKET_UDP_H__
#define __SOCKET_UDP_H__

#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "base.h"

typedef struct{
	s32_t sock_fd;
	s32_t dest_len;
	struct sockaddr_in addr_serv;
}sUdp_t;

sUdp_t *udp_create(s8_t *ip, u32_t port);
void udp_distory(sUdp_t *socket);
s32_t udp_send(sUdp_t *socket, s8_t *buf, u16_t len);
s32_t udp_recv(sUdp_t *socket, s8_t *buf);

#endif /* __SOCKET_UDP_H__ */
