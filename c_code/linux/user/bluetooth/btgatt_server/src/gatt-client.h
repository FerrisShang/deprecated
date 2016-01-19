/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2014  Google Inc.
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define BT_GATT_UUID_SIZE 16

struct bt_gatt_client *bt_gatt_client_new(struct gatt_db *db,
							struct bt_att *att,
							uint16_t mtu);

struct bt_gatt_client *bt_gatt_client_ref(struct bt_gatt_client *client);
void bt_gatt_client_unref(struct bt_gatt_client *client);

typedef void (*bt_gatt_client_destroy_func_t)(void *user_data);
typedef void (*bt_gatt_client_callback_t)(bool success, uint8_t att_ecode,
							void *user_data);
typedef void (*bt_gatt_client_debug_func_t)(const char *str, void *user_data);
typedef void (*bt_gatt_client_read_callback_t)(bool success, uint8_t att_ecode,
					const uint8_t *value, uint16_t length,
					void *user_data);
typedef void (*bt_gatt_client_write_long_callback_t)(bool success,
					bool reliable_error, uint8_t att_ecode,
					void *user_data);
typedef void (*bt_gatt_client_notify_callback_t)(uint16_t value_handle,
					const uint8_t *value, uint16_t length,
					void *user_data);
typedef void (*bt_gatt_client_register_callback_t)(uint16_t att_ecode,
							void *user_data);
typedef void (*bt_gatt_client_service_changed_callback_t)(uint16_t start_handle,
							uint16_t end_handle,
							void *user_data);
struct bt_gatt_client {
	struct bt_att *att;
	int ref_count;

	bt_gatt_client_callback_t ready_callback;
	bt_gatt_client_destroy_func_t ready_destroy;
	void *ready_data;

	bt_gatt_client_service_changed_callback_t svc_chngd_callback;
	bt_gatt_client_destroy_func_t svc_chngd_destroy;
	void *svc_chngd_data;

	bt_gatt_client_debug_func_t debug_callback;
	bt_gatt_client_destroy_func_t debug_destroy;
	void *debug_data;

	struct gatt_db *db;
	bool in_init;
	bool ready;

	/*
	 * Queue of long write requests. An error during "prepare write"
	 * requests can result in a cancel through "execute write". To prevent
	 * cancelation of prepared writes to the wrong attribute and multiple
	 * requests to the same attribute that may result in a corrupted final
	 * value, we avoid interleaving prepared writes.
	 */
	struct queue *long_write_queue;
	bool in_long_write;

	unsigned int reliable_write_session_id;

	unsigned int req_mtu_id;

	/* List of registered disconnect/notification/indication callbacks */
	struct queue *notify_list;
	struct queue *notify_chrcs;
	int next_reg_id;
	unsigned int disc_id, notify_id, ind_id;

	/*
	 * Handles of the GATT Service and the Service Changed characteristic
	 * value handle. These will have the value 0 if they are not present on
	 * the remote peripheral.
	 */
	unsigned int svc_chngd_ind_id;
	bool svc_chngd_registered;
	struct queue *svc_chngd_queue;  /* Queued service changed events */
	bool in_svc_chngd;

	/*
	 * List of pending read/write operations. For operations that span
	 * across multiple PDUs, this list provides a mapping from an operation
	 * id to an ATT request id.
	 */
	struct queue *pending_requests;
	unsigned int next_request_id;

	struct bt_gatt_request *discovery_req;
	unsigned int mtu_req_id;
};



bool bt_gatt_client_is_ready(struct bt_gatt_client *client);
bool bt_gatt_client_set_ready_handler(struct bt_gatt_client *client,
					bt_gatt_client_callback_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);
bool bt_gatt_client_set_service_changed(struct bt_gatt_client *client,
			bt_gatt_client_service_changed_callback_t callback,
			void *user_data,
			bt_gatt_client_destroy_func_t destroy);
bool bt_gatt_client_set_debug(struct bt_gatt_client *client,
					bt_gatt_client_debug_func_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);

uint16_t bt_gatt_client_get_mtu(struct bt_gatt_client *client);
struct gatt_db *bt_gatt_client_get_db(struct bt_gatt_client *client);

bool bt_gatt_client_cancel(struct bt_gatt_client *client, unsigned int id);
bool bt_gatt_client_cancel_all(struct bt_gatt_client *client);

unsigned int bt_gatt_client_read_value(struct bt_gatt_client *client,
					uint16_t value_handle,
					bt_gatt_client_read_callback_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);
unsigned int bt_gatt_client_read_long_value(struct bt_gatt_client *client,
					uint16_t value_handle, uint16_t offset,
					bt_gatt_client_read_callback_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);
unsigned int bt_gatt_client_read_multiple(struct bt_gatt_client *client,
					uint16_t *handles, uint8_t num_handles,
					bt_gatt_client_read_callback_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);

unsigned int bt_gatt_client_write_without_response(
					struct bt_gatt_client *client,
					uint16_t value_handle,
					bool signed_write,
					const uint8_t *value, uint16_t length);
unsigned int bt_gatt_client_write_value(struct bt_gatt_client *client,
					uint16_t value_handle,
					const uint8_t *value, uint16_t length,
					bt_gatt_client_callback_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);
unsigned int bt_gatt_client_write_long_value(struct bt_gatt_client *client,
				bool reliable,
				uint16_t value_handle, uint16_t offset,
				const uint8_t *value, uint16_t length,
				bt_gatt_client_write_long_callback_t callback,
				void *user_data,
				bt_gatt_client_destroy_func_t destroy);
unsigned int bt_gatt_client_prepare_write(struct bt_gatt_client *client,
				unsigned int id,
				uint16_t value_handle, uint16_t offset,
				const uint8_t *value, uint16_t length,
				bt_gatt_client_write_long_callback_t callback,
				void *user_data,
				bt_gatt_client_destroy_func_t destroy);
unsigned int bt_gatt_client_write_execute(struct bt_gatt_client *client,
					unsigned int id,
					bt_gatt_client_callback_t callback,
					void *user_data,
					bt_gatt_client_destroy_func_t destroy);

unsigned int bt_gatt_client_register_notify(struct bt_gatt_client *client,
				uint16_t chrc_value_handle,
				bt_gatt_client_register_callback_t callback,
				bt_gatt_client_notify_callback_t notify,
				void *user_data,
				bt_gatt_client_destroy_func_t destroy);
bool bt_gatt_client_unregister_notify(struct bt_gatt_client *client,
							unsigned int id);

bool bt_gatt_client_set_security(struct bt_gatt_client *client, int level);
int bt_gatt_client_get_security(struct bt_gatt_client *client);
