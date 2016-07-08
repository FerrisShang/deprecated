#ifndef __GATT_H__
#define __GATT_H__

#include "queue.h"
#include "bt_util.h"
#include "uuid.h"

typedef enum {
	GATT_SUCCESS         = 0,
	GATT_FAILED_BUSY     = -1,
	GATT_FAILED_NOEXIST  = -2,
	GATT_FAILED_TIMEOUT  = -3,
} gatt_status_t;

struct gatt_services;
struct gatt_service_cb {
	void (*onConnectionStateChange)(bdaddr_t *addr, int newState, void *pdata);
	void (*onCharacterRead)(bdaddr_t *addr, bt_uuid_t *chac_uuid, void *pdata);
	void (*onCharacterWrite)(bdaddr_t *addr, bt_uuid_t *chac_uuid,
			char *buf, UINT16 len, void *pdata);
	void (*onDescriptorRead)(bdaddr_t *addr, bt_uuid_t *desc_uuid, void *pdata);
	void (*onDescriptorWrite)(bdaddr_t *addr, bt_uuid_t *desc_uuid,
			char *buf, UINT16 len, void *pdata);
	void (*onMtuChanged)(bdaddr_t *addr, int mtu, void *pdata);
	//void onIndicationSent(bdaddr_t *addr, int status, void *pdata);
};
struct gatt_client_cb {
	/* unfinished */
};
struct gatt_character {
	bt_uuid_t uuid;
	UINT8 prop;
};
struct gatt_service {
	int (*sendIndication)(bdaddr_t *addr, char *buf, UINT16 len);
	int (*sendNotification)(bdaddr_t *addr, char *buf, UINT16 len/*, callback*/);
	UINT16 handle_start;
	UINT16 handle_end;
	bt_uuid_t uuid;
	struct queue *character_list; /* type of struct gatt_character */
	void *pdata;
};
struct gatt_service* create_service(bt_uuid_t *uuid);
struct gatt_character* create_character(bt_uuid_t *uuid, UINT8 prop);
int service_add_character(struct gatt_service *service, struct gatt_character *character);
const struct gatt_service* register_gatt_service(
		struct gatt_service* gatt_service, struct gatt_service_cb *io_cb, void *pdata);
const struct gatt_client* register_gatt_client(struct gatt_client_cb *io_cb, void *pdata);
int init_gatt(int hdev);

#endif /* __GATT_H__ */
