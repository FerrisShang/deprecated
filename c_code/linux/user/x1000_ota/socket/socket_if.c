#include "socket_if.h"

#if SOCKET_TYPE == 0 /* UDP */

sSocket_t *socket_create(s8_t *ip, u32_t port)
{
	return udp_create(ip, port);
}

void socket_distory(sSocket_t *socket)
{
	udp_distory(socket);
}

s32_t socket_send(sSocket_t *socket, s8_t *buf, u16_t len)
{
	return udp_send(socket, buf, len);
}

s32_t socket_recv(sSocket_t *socket, s8_t *buf)
{
	return udp_recv(socket, buf);
}

#else /* TCP */

sSocket_t *socket_create(s8_t *ip, u32_t port)
{
	dbg_print(DBG_WARNING, SOCK_IF_DBG, "unsupport socket mode.\n");
	return NULL;
}

void socket_distory(sSocket_t *socket)
{
	return;
}

s32_t socket_send(sSocket_t *socket, s8_t *buf, u16_t len)
{
	return NULL;
}

s32_t socket_recv(sSocket_t *socket, s8_t *buf)
{
	return NULL;
}
#endif /* SOCKET_TYPE == 0 */
