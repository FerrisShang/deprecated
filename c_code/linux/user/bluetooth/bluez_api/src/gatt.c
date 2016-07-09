#include <stdio.h>
#include "att.h"
#include "gatt.h"
#include "log.h"
#include "bt_mutex.h"

struct common_rsp_func {
};
struct gatt_service {
	//int (*sendIndication)(bdaddr_t *addr, char *buf, UINT16 len);
	//int (*sendNotification)(bdaddr_t *addr, char *buf, UINT16 len/*, callback*/);
	UINT16 handle_start;
	UINT16 handle_end;
	bt_uuid_t uuid;
	struct queue *character_list; /* type of struct gatt_character */
	struct gatt_service_cb *io_cb;
	void *pdata;
};
struct gatt_services {
	bt_mutex_t gatt_mutex;
	struct queue *service_list; /* type of struct gatt_service */
	struct common_rsp_func *common_rsp_func;
};

static struct gatt_services* create_services(void);
static void conn_change_cb(bdaddr_t addr, int status, void *pdata);
static int onReceive(bdaddr_t *addr, UINT8 opcode,
		const void *pdu, UINT16 length, void *pdata);
struct att_cb att_cb = {
	conn_change_cb,
	onReceive,
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
	if(!gatt_services){
		goto create_services_failed;
	}
	if(!bt_mutex_init(&gatt_services->gatt_mutex)){
		goto mutex_init_failed;
	}
	att = register_att(hdev, &att_cb, gatt_services);
	if(!att){
		goto register_att_failed;
	}
	return GATT_SUCCESS;
register_att_failed :
mutex_init_failed :
create_services_failed :
	return GATT_FAILED_NOMEM;
}

struct device_status {
	bdaddr_t *addr;
	int status;
};
static void gatt_conn_change_cb(void *data, void *user_data)
{
	struct gatt_service *gatt_service = data;
	struct device_status *dev_status = user_data;
	gatt_service->io_cb->onConnectionStateChange(
			dev_status->addr, dev_status->status, gatt_service->pdata);
}
static void conn_change_cb(bdaddr_t addr, int status, void *pdata)
{
	struct gatt_services *gatt_services = pdata;
	struct device_status dev_status;
	dev_status.addr = &addr; 
	dev_status.status = status; 
	queue_foreach(gatt_services->service_list, gatt_conn_change_cb, &dev_status);
}

struct handle_info {
	struct gatt_service *gatt_service;
	struct gatt_character *gatt_character;
	handle_type_t type;
};
bool find_gatt_service_by_handle_cb(const void *data, const void *match_data)
{
	struct gatt_service *gatt_service = (struct gatt_service*)data;
	UINT16 *handle = (UINT16*) match_data;
	if(gatt_service->handle_start <= *handle &&
			gatt_service->handle_end >= *handle){
		return true;
	}else{
		return false;
	}
}
bool find_gatt_character_by_handle_cb(const void *data, const void *match_data)
{
	return true;
}
static int get_handle_info(struct handle_info *handle_info, UINT16 handle)
{
	struct gatt_service *gatt_service;
	struct gatt_character *gatt_character;
	bt_mutex_t *mutex;
	memset(handle_info, 0, sizeof(struct handle_info));
	if(!gatt_services){
		Log.e("gatt server not inited");
		goto services_no_exsit_failed;
	}
	mutex = &gatt_services->gatt_mutex;
	bt_mutex_lock(mutex);
	gatt_service = queue_find(gatt_services->service_list,
			find_gatt_service_by_handle_cb, &handle);
	if(!gatt_service){
		goto get_handle_info_failed;
	}
	handle_info->gatt_service = gatt_service;
	gatt_character = queue_find(gatt_service->character_list,
			find_gatt_character_by_handle_cb, &handle);
	if(!gatt_character){
		goto get_handle_info_failed;
	}
	handle_info->gatt_character = gatt_character;
	if(handle == gatt_character->value_handle){
		handle_info->type = HANDLE_CHARACTER;
	}else if(handle == gatt_character->desc_handle){
		handle_info->type = HANDLE_DESC_CONF;
	}else{
		goto get_handle_info_failed;
	}
	bt_mutex_unlock(mutex);
	return GATT_SUCCESS;
get_handle_info_failed :
	bt_mutex_unlock(mutex);
services_no_exsit_failed :
	return GATT_FAILED_NOEXIST;
}

static int onReceive(bdaddr_t *addr, UINT8 opcode,
		const void *pdu, UINT16 length, void *pdata)
{
	struct gatt_services *gatt_services = pdata;
	static UINT8 rsq_pdu[1024];
	static UINT16 rsq_len;
	UINT8 req_opcode = get_req_opcode(opcode);
	rsq_len = 0;
	UINT8 *p = (UINT8*)pdu;
	switch(opcode){
		case BT_ATT_OP_ERROR_RSP :
			break;
		case BT_ATT_OP_MTU_REQ :
			break;
		case BT_ATT_OP_MTU_RSP :
			break;
		case BT_ATT_OP_FIND_INFO_REQ :
			break;
		case BT_ATT_OP_FIND_INFO_RSP :
			break;
		case BT_ATT_OP_FIND_BY_TYPE_VAL_REQ :
			break;
		case BT_ATT_OP_FIND_BY_TYPE_VAL_RSP :
			break;
		case BT_ATT_OP_READ_BY_TYPE_REQ :
			break;
		case BT_ATT_OP_READ_BY_TYPE_RSP :
			break;
		case BT_ATT_OP_READ_BY_GRP_TYPE_REQ :
			break;
		case BT_ATT_OP_READ_BY_GRP_TYPE_RSP :
			break;
		case BT_ATT_OP_READ_REQ :
		case BT_ATT_OP_WRITE_REQ :{
			UINT16 handle;
			struct handle_info handle_info;
			STREAM_TO_UINT16(handle, p);
			if(get_handle_info(&handle_info, handle) == GATT_SUCCESS){
				switch(handle_info.type){
					//the end of read & write need process
					case HANDLE_CHARACTER :
						if(opcode == BT_ATT_OP_WRITE_REQ){
							handle_info.gatt_service->io_cb->onCharacterWrite(
									addr,
									&handle_info.gatt_character->uuid,
									p,
									length - ((UINT8*)p - (UINT8*)pdu),
									handle_info.gatt_service->pdata);
						}else if(opcode == BT_ATT_OP_READ_REQ){
							handle_info.gatt_service->io_cb->onCharacterRead(
									addr, &handle_info.gatt_character->uuid,
									handle_info.gatt_service->pdata);
						}
						break;
					case HANDLE_DESC_CONF :
						if(opcode == BT_ATT_OP_WRITE_REQ){
							handle_info.gatt_service->io_cb->onDescriptorWrite(
									addr,
									&handle_info.gatt_character->uuid,
									p,
									length - ((UINT8*)p - (UINT8*)pdu),
									handle_info.gatt_service->pdata);
						}else if(opcode == BT_ATT_OP_READ_REQ){
							handle_info.gatt_service->io_cb->onDescriptorRead(
									addr, &handle_info.gatt_character->uuid,
									handle_info.gatt_service->pdata);
						}
						break;
					default :
						break;
				}
			}else{
				Log.e("handle not found");
			}
			}break;
		case BT_ATT_OP_HANDLE_VAL_NOT :
		case BT_ATT_OP_HANDLE_VAL_IND :
		case BT_ATT_OP_READ_RSP :
		case BT_ATT_OP_WRITE_RSP :
			//use for client
			break;
		case BT_ATT_OP_READ_BLOB_REQ :
		case BT_ATT_OP_READ_BLOB_RSP :
		case BT_ATT_OP_READ_MULT_REQ :
		case BT_ATT_OP_READ_MULT_RSP :
		case BT_ATT_OP_WRITE_CMD :
		case BT_ATT_OP_SIGNED_WRITE_CMD :
		case BT_ATT_OP_PREP_WRITE_REQ :
		case BT_ATT_OP_PREP_WRITE_RSP :
		case BT_ATT_OP_EXEC_WRITE_REQ :
		case BT_ATT_OP_EXEC_WRITE_RSP :
		case BT_ATT_OP_HANDLE_VAL_CONF :
		default:
			Log.e("ATT opcode not support : %d", opcode);
			break;
	}
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

void init_service_handle_value_cb(void *data, void *user_data)
{
	struct gatt_character *gatt_character = data;
	struct gatt_service *gatt_service = user_data;
	if(gatt_character->prop & BT_GATT_CHRC_PROP_NOTIFY ||
		gatt_character->prop & BT_GATT_CHRC_PROP_INDICATE ){
		gatt_character->value_handle = gatt_service->handle_end + 2;
		gatt_character->desc_handle = gatt_service->handle_end + 3;
		gatt_service->handle_end += 3;
	}else{
		gatt_character->value_handle = gatt_service->handle_end + 2;
		gatt_character->desc_handle = 0;
		gatt_service->handle_end += 2;
	}
}
static int init_service_handle_value(struct gatt_services* gatt_services,
		struct gatt_service *gatt_service)
{
	UINT16 last_handle_value;
	struct gatt_service *last_gatt_service;
	last_gatt_service = queue_peek_tail(gatt_services->service_list);
	if(!last_gatt_service){
		last_handle_value = 1;
	}else{
		last_handle_value = last_gatt_service->handle_end;
	}
	if(last_handle_value >= 0xFFFF){
		return GATT_FAILED_NOMEM;
	}
	gatt_service->handle_start = last_handle_value + 1;
	gatt_service->handle_end = gatt_service->handle_start;
	queue_foreach(gatt_service->character_list, init_service_handle_value_cb, gatt_service);
	return GATT_SUCCESS;
}
const struct gatt_service* register_gatt_service(
		struct gatt_service* gatt_service, struct gatt_service_cb *io_cb, void *pdata)
{
	bt_mutex_t *mutex;
	if(!gatt_services){
		Log.e("gatt server not inited");
		return NULL;
	}
	mutex = &gatt_services->gatt_mutex;
	bt_mutex_lock(mutex);
	init_service_handle_value(gatt_services, gatt_service);
	gatt_service->io_cb = io_cb;
	gatt_service->pdata = pdata;
	queue_push_tail(gatt_services->service_list, gatt_service);
	bt_mutex_unlock(mutex);
	return NULL;
}
const struct gatt_client* register_gatt_client(struct gatt_client_cb *io_cb, void *pdata)
{
	return NULL;
}
