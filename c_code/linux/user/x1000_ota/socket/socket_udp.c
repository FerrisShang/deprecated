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

	udp_handle->sock_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(udp_handle->sock_fd){
		udp_handle->addr_serv.sin_family = AF_INET;
		udp_handle->addr_serv.sin_addr.s_addr = inet_addr((const char*)ip);
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
	return 0;
}

s32_t udp_recv(sUdp_t *socket, s8_t *buf)
{
	return 0;
}
