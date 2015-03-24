#include <stdlib.h>
#include <stdio.h>
#define led_init() \
	*(volatile int*)0xb0010274 = (1<<5)+(1<<4)+(1<<13); \
	*(volatile int*)0xb0010218 = (1<<5)+(1<<4)+(1<<13); \
	*(volatile int*)0xb0010224 = (1<<5)+(1<<4)+(1<<13); \
	*(volatile int*)0xb0010238 = (1<<5)+(1<<4)+(1<<13); \
	*(volatile int*)0xb0010248 = (1<<5)+(1<<4)+(1<<13);
#define ledr(state) *(volatile int*)(0xb0010248-4*(!!(state))) = (1<<5)
#define ledg(state) *(volatile int*)(0xb0010248-4*(!!(state))) = (1<<4)
#define ledb(state) *(volatile int*)(0xb0010248-4*(!!(state))) = (1<<13)
extern _start;
void put_str(char *str)
{
	while(*str)
		putchar(*str++);
}

void put_addr(int num)
{
	char i;
	unsigned char ch[8];
	putchar('0');
	putchar('x');
	for(i=0;i<8;i++){
		ch[i] = num & 0x0F;
		num >>= 4;
	}
	for(i=7;i>=0;i--){
		if(ch[i]>=10)
			putchar(ch[i]-10+'A');
		else
			putchar(ch[i]+'0');
	}
}
void main(void)
{
	char buf[80];
	int tmp;
	led_init();
	ledr(_start & 0x00000400);
	ledg(_start & 0x00000200);
	ledb(_start & 0x00000100);
	put_str("\n\rhello world\n\r,start=");
	put_addr(_start);
	put_str("\n\r");
	asm("sw sp, tmp");
	put_addr(tmp);
	put_str("\n\rdone..\n\r");
}	









