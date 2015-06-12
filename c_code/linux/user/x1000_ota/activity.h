#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__
#include "base.h"
#include "protocol.h"
#include "socket_if.h"
#include "ota_para.h"

s32_t activity_device(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para);

#define ACTIVITY_SUCCESS    0
#define ACTIVITY_ERROR     -1

#define ACT_RECV_TIMEOUT_S  3ul
#endif /* __ACTIVITY_H__ */
