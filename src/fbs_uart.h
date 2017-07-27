#ifndef FBS_UART_H__
#define FBS_UART_H__

void FBS_enable_btsnoop(const gchar *path);
void* FBS_uart_recv(GMainLoop *mainloop);
guint FBS_uart_send(guchar *data, guint len);
void* fbs_command_recv(void *p); //RFU

#endif /* FBS_UART_H__ */
