#include <stdio.h>
#include <unistd.h>
#include "uart_hci.h"
#include "bluetooth.h"

int test_func(struct bt_msg* bt_msg)
{
	printf("test_func(but return -1)\n");
	return -1;
}
struct bt_msg_proc_node test_node = {
	test_func,
	BT_MSG_CB_ONCE,
};

int main(int argc, char *argv[])
{
	char buf[]={1,3,12,0};
	char buf1[]={1,5,16,0};
	struct uart_hci *uart_hci;
	uart_hci = create_uart(UART_DEF_PORT, UART_DEF_BAUD, uart_process);
	if(uart_hci == NULL){
		printf("create uart hci failed\n");
		return 1;
	}
	sleep(1);
	printf("write:%d\n", write_hci(uart_hci, buf, sizeof(buf)));
	sleep(1);
	bt_hci_proc_reg(&test_node);
	printf("write:%d\n", write_hci(uart_hci, buf1, sizeof(buf1)));
	sleep(10);
	destory_uart(uart_hci);
	return 0;
}
