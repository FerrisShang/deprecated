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
}eOta_ins_t;

typedef enum{
	TYPE_ENCRYPT_NULL = 0,
	TYPE_ENCRYPT_RSA
	TYPE_ENCRYPT_AES,
}eEncrypt_t;

void parse_valid_data(void);
void pack_valid_data(void);

void parse_comm_data(void);
void pack_comm_data(void);

#define COMM_DATA_HEADER_POS     0
#define COMM_DATA_TYPE_POS       1
#define COMM_DATA_LEN_POS        2
#define COMM_DATA_VALID_DATA_POS 4

#define COMM_DATA_HEADER        "X"

#define VALID_DATA_HEADER_POS     0
#define VALID_DATA_LEN_POS        2
#define VALID_DATA_TIME_STAMP     4
#define VALID_DATA_INS            8
#define VALID_DATA_DATA           10

#define VALID_DATA_HEADER     "JZ"

#endif /* __PROTOCOL_H__ */
