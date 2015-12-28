#include <stdio.h>
#include <unistd.h>
#include "uart_hci.h"
#include "bluetooth.h"
#include "bt_semaphore.h"

typedef struct {
	int res;
	bt_sem_t sem;
}pdata_t;
pdata_t pdata;
int callback(struct bt_pkg* bt_pkg, void *pdata)
{
	printf("xx");
	pdata_t *pd = pdata;
	bt_sem_post(&pd->sem);
	return -1;
}
struct bt_pkg_proc_node proc_node = {
	callback,
	&pdata,
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
	bt_sem_init(&pdata.sem, 0);
	bt_hci_proc_reg(&proc_node);
	write_hci(uart_hci, buf, sizeof(buf));
	//bt_sem_wait(&pdata.sem);
	bt_sem_timedwait(&pdata.sem, 1);

	write_hci(uart_hci, buf1, sizeof(buf1));
	sleep(1);
	destory_uart(uart_hci);
	return 0;
}
