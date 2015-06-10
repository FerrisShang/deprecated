#include <stdio.h>
#include <string.h>
#include "socket_udp.h"

sUdp_t *udp_create(s8_t *ip, u32_t port)
{
	sUdp_t *udp_handle;
	udp_handle = (sUdp_t*)mem_malloc(sizeof(sUdp_t));
	if(udp_handle == NULL){
		dbg_print(DBG_ERROR, SOCK_UDP_DBG, "not enough memory.\n");
		return NULL;
	}
	memset(udp_handle, 0,sizeof(sUdp_t));

	udp_handle->dest_len = sizeof(struct sockaddr_in);
	udp_handle->sock_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(udp_handle->sock_fd){
		udp_handle->addr_serv.sin_family = AF_INET;
		udp_handle->addr_serv.sin_addr.s_addr = inet_addr((const s8_t*)ip);
		udp_handle->addr_serv.sin_port = htons(port);
	}else{
		dbg_print(DBG_ERROR, SOCK_UDP_DBG, "allocate socket failed.\n");
		mem_free(udp_handle);
		return NULL;
	}
	return udp_handle;
}

void udp_distory(sUdp_t *socket)
{
	if(socket == NULL){
		dbg_print(DBG_WARNING, SOCK_UDP_DBG, "attempt to free zero pointer.\n");
	}else{
		if(socket->sock_fd){
			close(socket->sock_fd);
		}
		mem_free(socket);
	}
}

s32_t udp_send(sUdp_t *socket, s8_t *buf, u16_t len)
{
	s32_t send_num = -1;
	if(socket != NULL){
		if(socket->sock_fd){
			send_num = sendto(socket->sock_fd, buf, len, 0,
					(struct sockaddr *)&socket->addr_serv, socket->dest_len);
			return send_num;
		}else{
			dbg_print(DBG_WARNING, SOCK_UDP_DBG, "sock_fd error.\n");
			return -1;
		}
	}else{
		dbg_print(DBG_WARNING, SOCK_UDP_DBG, "socket error.\n");
		return -1;
	}
}

s32_t udp_recv(sUdp_t *socket, s8_t *buf, s32_t len)
{
	s32_t recv_num = -1;
	if(socket != NULL){
		if(socket->sock_fd){
			recv_num = recvfrom(socket->sock_fd, buf, len, MSG_DONTWAIT,
					(struct sockaddr *)&socket->addr_serv, &socket->dest_len);
			return recv_num;
		}else{
			dbg_print(DBG_WARNING, SOCK_UDP_DBG, "sock_fd error.\n");
			return -1;
		}
	}else{
		dbg_print(DBG_WARNING, SOCK_UDP_DBG, "socket error.\n");
		return -1;
	}
}
