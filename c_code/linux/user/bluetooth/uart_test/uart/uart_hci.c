#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "base.h"
#include "uart_hci.h"

static void uart_recv_callback(void *para);

struct uart_hci *create_uart(char *port, int baud,
		int (*recv_callback)(char *buf, int len))
{
	struct uart_hci *uhci;
	int ret;

	if(recv_callback == 0){
		return NULL;
	}
	uhci = mem_malloc(sizeof(struct uart_hci));
	if(uhci == NULL){
		dbg_print(DBG_ERROR, UART_HCI_DBG, "malloc error\n");
		goto err_malloc_uhci;
	}
	uhci->recv_callback = recv_callback;
	uhci->recv_buf = mem_malloc(UART_RECV_BUF_SIZE);
	if(uhci->recv_buf == NULL){
		dbg_print(DBG_ERROR, UART_HCI_DBG, "malloc error\n");
		goto err_malloc_buf;
	}	
	if ((uhci->fd = open(port, O_RDWR | O_NOCTTY)) < 0){
		dbg_print(DBG_ERROR, UART_HCI_DBG, "open device error\n");
		goto err_open;
	}
	ret = pthread_create(&uhci->recv_thrd, NULL, (void *)&uart_recv_callback, (void *)uhci);
	if(ret != 0){
		dbg_print(DBG_ERROR, UART_HCI_DBG, "create thread failed\n");
		goto err_thread;
	}
	return uhci;
err_thread:
	close(uhci->fd);
err_open:
	mem_free(uhci->recv_buf);
err_malloc_buf:
	mem_free(uhci);
err_malloc_uhci:
	return NULL;
}

int write_hci(struct uart_hci *uhci, char *buf, int len)
{
	return write(uhci->fd, buf, len);
}

int destory_uart(struct uart_hci *uhci)
{
	if(pthread_kill(uhci->recv_thrd, 0) != 0){
		dbg_print(DBG_ERROR, UART_HCI_DBG, "stop thread failed.\n");
	}
	close(uhci->fd);
	mem_free(uhci);
	return 0;
}

static void uart_recv_callback(void *para)
{
	struct uart_hci *uhci;
	char *buf;
	int len;
	uhci = (struct uart_hci*)para;
	buf = uhci->recv_buf;
	while(1){
		len = read(uhci->fd, buf, UART_RECV_BUF_SIZE);
		if(len > 0){
			uhci->recv_callback(buf, len);
		}else{
			dbg_print(DBG_ERROR, UART_HCI_DBG, "read uart data error.\n");
			pthread_exit(0);
		}
	}
}
