#include <stdio.h>
#include <unistd.h>
#include "uart_hci.h"
#include "bluetooth.h"

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
	write_hci(uart_hci, buf, sizeof(buf));
	sleep(1);
	write_hci(uart_hci, buf1, sizeof(buf1));
	sleep(1);
	destory_uart(uart_hci);
	return 0;
}
