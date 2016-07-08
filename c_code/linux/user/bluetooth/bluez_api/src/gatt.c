#include <stdio.h>
#include "att.h"
#include "gatt.h"
#include "log.h"

struct gatt_services {
	struct queue *service_list; /* type of struct gatt_service */
};

static struct gatt_services* create_services(void);
static void conn_change_cb(bdaddr_t addr, int status, void *pdata);
static int onRequest(bdaddr_t *addr, UINT8 opcode,
		const void *pdu, UINT16 length, void *pdata);
struct att_cb att_cb = {
	conn_change_cb,
	onRequest,
};

/*
	int (*connect)(bdaddr_t *addr);
	int (*disconnect)(bdaddr_t *addr);
	int (*send)(bdaddr_t *addr, struct att_send_op *att_send_op);
*/
static const struct att *att;
static struct gatt_services *gatt_services;
int init_gatt(int hdev)
{
	gatt_services = create_services();
	att = register_att(hdev, &att_cb, gatt_services);
	return GATT_SUCCESS;
}

static void conn_change_cb(bdaddr_t addr, int status, void *pdata)
{
	struct gatt_services *gatt_services = pdata;
}

static int onRequest(bdaddr_t *addr, UINT8 opcode,
		const void *pdu, UINT16 length, void *pdata)
{
	struct gatt_services *gatt_services = pdata;
	return 0;
}

static struct gatt_services* create_services(void)
{
	struct gatt_services *gatt_services;
	gatt_services = mem_malloc(sizeof(struct gatt_services));
	if(!gatt_services){
		goto malloc_gatt_failed;
	}
	gatt_services->service_list = queue_new();
	if(!gatt_services->service_list){
		goto malloc_service_list_failed;
	}
	return gatt_services;
malloc_service_list_failed :
	mem_free(gatt_services);
malloc_gatt_failed :
	return NULL;
}
struct gatt_service* create_service(bt_uuid_t *uuid)
{
	return NULL;
}
struct gatt_character* create_character(bt_uuid_t *uuid, UINT8 prop)
{
	return NULL;
}
int service_add_character(struct gatt_service *service, struct gatt_character *character)
{
	return 0;
}
const struct gatt_service* register_gatt_service(
		struct gatt_service* gatt_service, struct gatt_service_cb *io_cb, void *pdata)
{
	return NULL;
}
const struct gatt_client* register_gatt_client(struct gatt_client_cb *io_cb, void *pdata)
{
	return NULL;
}

