#ifndef __UART_HCI_H__
#define __UART_HCI_H__

#include <pthread.h>

#define UART_DEF_PORT "/dev/ttyS1"
#define UART_DEF_BAUD 3000000
#define UART_RECV_BUF_SIZE 1024

struct uart_hci{
	int fd;
	pthread_t recv_thrd;
	int (*recv_callback)(char *buf, int len);
	char *recv_buf;
};

struct uart_hci *create_uart(char *port, int baud,
	   int (*recv_callback)(char *buf, int len));
int write_hci(struct uart_hci *uhci, char *buf, int len);
int destory_uart(struct uart_hci *uhci);

#endif /* __UART_HCI_H__ */
