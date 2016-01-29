#ifndef __MSG_HANDLE_H__
#define __MSG_HANDLE_H__

struct msg_handle;
typedef void (*msg_handle_cb_t)(int cmd, void *msg_data, void *user_data);

struct msg_handle *msg_handle_new(msg_handle_cb_t msg_handle_cb, void *user_data);
void msg_send(struct msg_handle *msg_handle, int cmd, void *msg_data);//cmd must be equal or lager than zero
void msg_handle_destory(struct msg_handle *msg_handle);

#endif /* __MSG_HANDLE_H__ */
