#ifndef __GATT_H__
#define __GATT_H__

#include "queue.h"
#include "bt_util.h"
#include "att.h"
#include "uuid.h"

#define DESCREPTOR_NOTIFICATION 0x0001
#define DESCREPTOR_INDICATION   0x0002

struct gatt_service;

typedef enum {
	GATT_SUCCESS         = 0,
	GATT_FAILED_BUSY     = -1,
	GATT_FAILED_NOEXIST  = -2,
	GATT_FAILED_TIMEOUT  = -3,
	GATT_FAILED_NOMEM    = -4,
} gatt_status_t;

struct gatts_if {
	int (*sendNotification)(bdaddr_t *addr, bt_uuid_t *chac_uuid, char *buf, UINT16 len);
	//int (*sendIndication)(bdaddr_t *addr, bt_uuid_t *chac_uuid, char *buf, UINT16 len);
	///*, callback*/);
};

struct gatts_cb {
	void (*onConnectionStateChange)(bdaddr_t *addr, int newState, void *pdata);
	void (*onCharacterRead)(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata,
			UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len);
	void (*onCharacterWrite)(bdaddr_t *addr, bt_uuid_t *chac_uuid,
			UINT8 *buf, UINT16 len, void *pdata);
	void (*onDescriptorRead)(bdaddr_t *addr, bt_uuid_t *desc_uuid, void *pdata,
			UINT8 *read_rsp_buf, UINT16 *read_rsp_buf_len);
	void (*onDescriptorWrite)(bdaddr_t *addr, bt_uuid_t *desc_uuid,
			UINT8 *buf, UINT16 len, void *pdata);
	void (*onMtuChanged)(bdaddr_t *addr, int mtu, void *pdata);
	//void onIndicationSent(bdaddr_t *addr, int status, void *pdata);
};
struct gatt_client_cb {
	/* unfinished */
};
struct gatt_character {
	UINT16 type_handle;
	UINT16 value_handle;
	UINT16 desc_handle;
	bt_uuid_t *uuid;
	UINT8 prop;
};

int init_gatt(int hdev);
//server api
struct gatt_service* create_service(bt_uuid_t *uuid);
struct gatt_character* create_character(bt_uuid_t *uuid, UINT8 prop);
int service_add_character(struct gatt_service *service, struct gatt_character *character);
const struct gatts_if* register_gatt_service(
		struct gatt_service* gatt_service, struct gatts_cb *io_cb, void *pdata);
//client api
const struct gatt_client* register_gatt_client(struct gatt_client_cb *io_cb, void *pdata);

#endif /* __GATT_H__ */
