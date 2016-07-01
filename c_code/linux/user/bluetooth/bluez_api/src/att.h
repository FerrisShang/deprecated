#ifndef __ATT_H__
#define __ATT_H__

#include "att_io.h"

#define ATT_REQUEST_TOUT_S              3
/* ATT protocol opcodes */
#define BT_ATT_OP_ERROR_RSP             0x01
#define BT_ATT_OP_MTU_REQ               0x02
#define BT_ATT_OP_MTU_RSP               0x03
#define BT_ATT_OP_FIND_INFO_REQ         0x04
#define BT_ATT_OP_FIND_INFO_RSP         0x05
#define BT_ATT_OP_FIND_BY_TYPE_VAL_REQ  0x06
#define BT_ATT_OP_FIND_BY_TYPE_VAL_RSP  0x07
#define BT_ATT_OP_READ_BY_TYPE_REQ      0x08
#define BT_ATT_OP_READ_BY_TYPE_RSP      0x09
#define BT_ATT_OP_READ_REQ              0x0a
#define BT_ATT_OP_READ_RSP              0x0b
#define BT_ATT_OP_READ_BLOB_REQ         0x0c
#define BT_ATT_OP_READ_BLOB_RSP         0x0d
#define BT_ATT_OP_READ_MULT_REQ         0x0e
#define BT_ATT_OP_READ_MULT_RSP         0x0f
#define BT_ATT_OP_READ_BY_GRP_TYPE_REQ  0x10
#define BT_ATT_OP_READ_BY_GRP_TYPE_RSP  0x11
#define BT_ATT_OP_WRITE_REQ             0x12
#define BT_ATT_OP_WRITE_RSP             0x13
#define BT_ATT_OP_WRITE_CMD             0x52
#define BT_ATT_OP_SIGNED_WRITE_CMD      0xD2
#define BT_ATT_OP_PREP_WRITE_REQ        0x16
#define BT_ATT_OP_PREP_WRITE_RSP        0x17
#define BT_ATT_OP_EXEC_WRITE_REQ        0x18
#define BT_ATT_OP_EXEC_WRITE_RSP        0x19
#define BT_ATT_OP_HANDLE_VAL_NOT        0x1B
#define BT_ATT_OP_HANDLE_VAL_IND        0x1D
#define BT_ATT_OP_HANDLE_VAL_CONF       0x1E

typedef enum {
	ATT_SUCCESS         = 0,
	ATT_FAILED_BUSY     = -1,
	ATT_FAILED_NOEXIST  = -2,
	ATT_FAILED_TIMEOUT  = -3,
} att_status_t;

typedef void (*att_response_func_t)(bdaddr_t addr, UINT8 opcode, const void *pdu,
		UINT16 length, void *pdata, att_status_t status);

struct att_send_op {
	uint16_t opcode;
	void *pdu;
	uint16_t len;
	att_response_func_t resp_cb;
	void *user_data;
};

enum{
	ATT_STATUS_CONNECTED,
	ATT_STATUS_DISCONNECTED,
};
struct att_cb {
	void (*conn_change_cb)(bdaddr_t addr, int status, void *pdata);
	int (*onRequest)(bdaddr_t *addr, UINT8 opcode,
			const void *pdu, UINT16 length, void *pdata);
};

struct att {
	int (*connect)(bdaddr_t *addr);
	int (*disconnect)(bdaddr_t *addr);
	int (*send)(bdaddr_t *addr, struct att_send_op *att_send_op);
};
const struct att* register_att(int hdev, struct att_cb *io_cb, void *pdata);

#endif /* __ATT_H__ */
