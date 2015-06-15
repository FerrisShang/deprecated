#ifndef __INIT_ENC_H__
#define __INIT_ENC_H__

#include "base.h"
#include "socket_if.h"
#include "protocol.h"
#include "ota_para.h"

#define INIT_ENC_SUCCESS    0
#define INIT_ENC_ERROR     -1

#define INIT_ENC_TIMEOUT_S     3ul
#define UPDATE_AES__TIMEOUT_S  3ul

s32_t req_enc_conn(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para);
s32_t req_new_aes_key(sProtocol_t *pro, sSocket_t *socket, sOta_para_t *ota_para);

#endif /* __INIT_ENC_H__ */
