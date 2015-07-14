#include <stdio.h>
#include <unistd.h>
#include "uart_hci.h"
#include "hci.h"

int find_cmd(char *buf, int len, char *pbuf)
{
}
int callback(char *buf, int len)
{
	int i;
	char *tbuf = buf;
	int tlen = len;
	for(i=0;i<len;i++){
		if ((i%16)==0)printf("\n");
		printf("%02x ", (unsigned char)buf[i]);
	}
	printf("\n");
	/*
	while(1){
		int sbuf = 0;
		sbuf = find_cmd(&slen, tbuf, tlen);
		if(slen > 0){
			//call other func;
			printf("call other func,len = %d\n", slen);
		}else if(slen == 0){
			break;
		}else{
			printf("wrong hci data\n");
		}
	}
	*/
	return 0;
}

int main(int argc, char *argv[])
{
	char buf[]={1,5,4,0x0d,0x7e,0x33,0xfb,0xe3,0x3a,0x8c,0x18,0,1,0,0,0,1};
	struct uart_hci *uart_hci;
	struct inBlue *inBlue;
	int len;
	uart_hci = create_uart(UART_DEF_PORT, UART_DEF_BAUD, callback);
	if(uart_hci == NULL){
		printf("create uart hci failed\n");
		return 1;
	}
	sleep(1);
	printf("write:%d\n", write_hci(uart_hci, buf, sizeof(buf)));
	sleep(10);
	destory_uart(uart_hci);
	return 0;
}
