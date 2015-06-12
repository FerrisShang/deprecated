#ifndef __CHK_VERSION__
#define __CHK_VERSION__

#include "base.h"
#include "protocol.h"
#include "socket_if.h"
#include "ota_para.h"

s32_t check_version(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para);

#define CHK_VERSION_HAVE_NEW  0
#define CHK_VERSION_NO_NEW   -1
#define CHK_VERSION_ERROR    -2

#define CHK_VERSION_TIMEOUT_S  3ul

#endif /* __CHK_VERSION__ */
