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
	TYPE_ENCRYPT_AUTO = 0xFF,//use for response
}eEncrypt_t;

typedef struct{
	eEncrypt_t enc;
	eOta_cmd_t cmd;
	u16_t data_len;
	void *data;
}sPro_udata_t;

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
	sValid_data_t valid_data;
	sComm_data_t comm_data;
	s32_t time_stamp_offset;
}sProtocol_t;

sProtocol_t* protocol_create(void);
void protocol_destory(sProtocol_t *pro);

s32_t proto_package_data(sProtocol_t *pro, sPro_udata_t *usr_data, s8_t *buf);
s32_t proto_parse_data(sProtocol_t *pro, sPro_udata_t *usr_data, s8_t *buf);

#define COMM_DATA_HEADER_POS     0
#define COMM_DATA_TYPE_POS       1
#define COMM_DATA_LEN_POS        2
#define COMM_DATA_VALID_DATA_POS 4

#define COMM_DATA_HEADER        "X"

#define VALID_DATA_HEADER_POS     0
#define VALID_DATA_LEN_POS        2
#define VALID_DATA_TIME_STAMP_POS 4
#define VALID_DATA_INS_POS        8
#define VALID_DATA_DATA_POS       10

#define VALID_DATA_HEADER     "JZ"

#define COMM_DATA_MAX_LEN     512
#define VALID_DATA_MAX_LEN    512

#endif /* __PROTOCOL_H__ */
