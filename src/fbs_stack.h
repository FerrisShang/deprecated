#ifndef FBS_STACK_H__
#define FBS_STACK_H__

#define FBS_ENV_DEV_PATH "FBS_ENV_DEV_PATH"
#define FBS_ENV_FW_PATH  "FBS_ENV_FW_PATH"
#define FBS_BTSNOOP_PATH "FBS_BTSNOOP_PATH"

typedef guchar bdaddr_t[6];
void FBS_stack_init_all(void);
void FBS_stack_run(void);
void FBS_stack_destroy_all(void);
void FBS_uart_send_raw(guchar *data, guint len);

#endif /* FBS_STACK_H__ */
