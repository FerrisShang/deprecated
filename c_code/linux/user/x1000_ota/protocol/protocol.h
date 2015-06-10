#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include "base.h"

typedef enum{
	CMD_REQ_ENCRYPT_COMM = 0x0001,
	CMD_ACK_ENCRYPT_COMM,
	CMD_INFO_KEY_EXPIRE,
	CMD_REQ_NEW_KEY,
	CMD_ACK_NEW_KEY,
	CMD_REQ_ACTIVITION,
	CMD_ACK_ACTIVITION,
	CMD_REQ_NEW_VERSION,
	CMD_ACK_NEW_VERSION,
}eOta_cmd_t;

typedef enum{
	TYPE_ENCRYPT_NULL = 0,
	TYPE_ENCRYPT_RSA,
	TYPE_ENCRYPT_AES,
	TYPE_ENCRYPT = 0xFF,//use for init
}eEncrypt_t;

typedef struct{
	u8_t header[2];
	u16_t len;
	u32_t time_stamp;
	eOta_cmd_t cmd;
	void *data;
	u16_t data_len;
	u8_t check_sum;
}sValid_data_t;

typedef struct{
	u8_t header[2];
	eEncrypt_t enc;
	u16_t len;
	void *data;
	u16_t data_len;
}sComm_data_t;

typedef struct{
	eEncrypt_t mode;
	u8_t *valid_dat_buf;
	s32_t time_stamp_offset;
}sProtocol_t;

sProtocol_t* protocol_create(eEncrypt_t mode);
void protocol_destory(sProtocol_t *pro);

s32_t pack_req_encrypt_comm(sProtocol_t *pro, u8_t *buf, u8_t *device_id);
s32_t pack_req_new_key(sProtocol_t *pro, u8_t *buf, u8_t *device_id);
s32_t pack_req_activition(sProtocol_t *pro, u8_t *buf, u8_t *device_id, u8_t *wifi_addr, u8_t *bt_addr);
s32_t pack_req_new_version(sProtocol_t *pro, u8_t *buf, u8_t *device_id, u8_t *version);

#define COMM_DATA_HEADER_POS     0
#define COMM_DATA_TYPE_POS       1
#define COMM_DATA_LEN_POS        2
#define COMM_DATA_VALID_DATA_POS 4

#define COMM_DATA_HEADER        "X"

#define VALID_DATA_OFFSET         4
#define VALID_DATA_HEADER_POS     0
#define VALID_DATA_LEN_POS        2
#define VALID_DATA_TIME_STAMP_POS 4
#define VALID_DATA_CMD_POS        8
#define VALID_DATA_DATA_POS       10

#define VALID_DATA_HEADER     "JZ"

#define COMM_DATA_MAX_LEN     512
#define VALID_DATA_MAX_LEN    512

#endif /* __PROTOCOL_H__ */
