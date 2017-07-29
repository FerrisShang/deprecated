#ifndef FBS_UART_H__
#define FBS_UART_H__

void  FBS_enable_btsnoop(const gchar *path);
void  FBS_uart_init(GMainLoop *mainloop,
		void (*recv_cb)(guchar *data, gint len, gpointer pdata),
		gpointer pdata);
void* FBS_uart_recv(void *unused);
gint  FBS_uart_send(guchar *data, gint len);

#endif /* FBS_UART_H__ */
