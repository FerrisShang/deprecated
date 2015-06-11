#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__
#include "base.h"
#include "protocol.h"
#include "socket_if.h"

s32_t activity_device(sProtocol_t *pro, sSocket_t *sock);

#define ACTIVITY_SUCCESS   0
#define ACTIVITY_ERROR    -1

#endif /* __ACTIVITY_H__ */
