#ifndef __SOCKET_TCP_H__
#define __SOCKET_TCP_H__

#include "base.h"

typedef struct{
	s32_t sock_fd;
	socklen_t dest_len;
	struct sockaddr_in addr_serv;
}sTcp_t;

#endif /* __SOCKET_TCP_H__ */
