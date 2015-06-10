#ifndef __SOCKET_IF_H__
#define __SOCKET_IF_H__

#include "base.h"

#if SOCKET_TYPE == 0

#include "socket_udp.h"
typedef sUdp_t sSocket_t;

#else

#include "socket_tcp.h"
typedef sTcp_t sSocket_t;

#endif /* SOCKET_TYPE == 0 */

sSocket_t *socket_create(s8_t *ip, u32_t port);
void socket_distory(sSocket_t *socket);
s32_t socket_send(sSocket_t *socket, s8_t *buf, s32_t len);
s32_t socket_recv(sSocket_t *socket, s8_t *buf, s32_t len);

#endif /* __SOCKET_IF_H__ */
