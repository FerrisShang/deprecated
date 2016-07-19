#ifndef __PROC_COMM_H__
#define __PROC_COMM_H__

#include "ipc_cmd.h"
#define MAX_CLIENT_ID_LEN 16

typedef ipc_cmd_io_cb pc_recv_cb;
typedef ipc_cmd_io_cb pc_send_out_cb;
typedef ipc_cmd_io_cb pc_send_return_cb;
struct client_id {
	char data[MAX_CLIENT_ID_LEN];
	int len;
};
struct pc_c_client;
struct pc_s_client;
struct pc_server;
typedef void (*pc_change_req_cb)(struct pc_s_client *client, int status, void *pdata);

//server api
struct pc_server* pc_create_server(key_t sem_key, key_t shm_key,
		int buffer_size,
		pc_recv_cb recv_cb,
		pc_change_req_cb conn_state_change_req,
		void *pdata);
int pc_server_run(struct pc_server *server);
int pc_destroy_server(struct pc_server* server);
int pc_destroy_client(struct pc_s_client* client);
int pc_s_send(struct pc_s_client* client,
		pc_send_out_cb out_cb, pc_send_return_cb ret_cb, void *pdata);

//client api
struct pc_c_client* pc_req_create_client(key_t sem_key, key_t shm_key,
		int buffer_size,
		void *client_id, int id_len,
		pc_recv_cb callback, void *pdata);
int pc_client_run(struct pc_c_client* client);
int pc_req_destroy_client(struct pc_c_client* client);
int pc_c_send(struct pc_c_client* client,
		pc_send_out_cb out_cb, pc_send_return_cb ret_cb, void *pdata);

#endif /* __PROC_COMM_H__ */
