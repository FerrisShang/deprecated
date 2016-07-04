#ifndef __GATT_H__
#define __GATT_H__

struct gatt_service* create_service(bt_uuid_t *uuid);
struct gatt_character* create_character(bt_uuid_t *uuid);
struct gatt_descriptor* create_descriptor(bt_uuid_t *uuid);
const struct gatt_server* register_gatt_server(int hdev, struct gatt_cb *io_cb, void *pdata);
const struct gatt_client* register_gatt_client(int hdev, struct gatt_cb *io_cb, void *pdata);

#endif /* __GATT_H__ */
