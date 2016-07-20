#ifndef __PROC_COMM_H__
#define __PROC_COMM_H__

#include <stdbool.h>
#include "ipc_cmd.h"
#define MAX_CLIENT_ID_LEN 16

struct pc_c_client;
struct pc_server;

//server api
typedef bool (*pc_is_broadcast_cb)(void *id, int id_len);
typedef bool (*pc_is_bc_match_cb)(void *id_bc, void *id, int id_len);
struct pc_server* pc_create_server(
		key_t sem_key, key_t shm_key, int buffer_size,
		pc_is_broadcast_cb is_broadcase_cb,
		pc_is_bc_match_cb is_bc_match_cb);
int pc_server_run(struct pc_server *server);
int pc_destroy_server(struct pc_server* server);

//client api
typedef void (*pc_recv_cb)(void *id, int id_len, void *buf, int buf_len, void *pdata);
typedef void (*pc_send_out_cb)(void *buf, int *buf_len, void *pdata);
typedef void (*pc_send_return_cb)(int status, void *pdata);
struct pc_c_client* pc_req_create_client(key_t sem_key, key_t shm_key,
		int buffer_size,
		void *client_id, int id_len,
		pc_recv_cb callback, void *pdata);
int pc_client_run(struct pc_c_client* client);
int pc_req_destroy_client(struct pc_c_client* client);
int pc_c_send(struct pc_c_client* client, char *id, int id_len,
		pc_send_out_cb out_cb, pc_send_return_cb ret_cb, void *pdata);

#endif /* __PROC_COMM_H__ */
