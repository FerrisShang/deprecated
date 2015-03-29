#include <stdlib.h>
#include <stdio.h>

void dbg_uart_init(void);
void dbg_uart_putc(char ch);
void put_str(char chn, char *str);
void put_char(char chn, char ch);
void put_addr(char chn, int num);

void main(void)
{
	int buf[80];
	int i, tmp;
	gpio_uart_init();
	dbg_uart_init();
	put_str(1, "gpio uart init ok.\r\n");
	put_str(0, "dbg uart init ok.\n");
}

#define SERIAL_IDX 3
#define SERIAL_BASE 0xB0030000
#define SERIAL_ADDR (SERIAL_BASE+0x1000*SERIAL_IDX)
#define REG_CLKGR 0xB0000020
#define SERIAL_BAUD 57600
#define SYS_EXTAL 24000000
void dbg_uart_putc(char ch)
{
	if(ch == '\n')
		dbg_uart_putc('\r');
	*(volatile int*)(SERIAL_ADDR + 0x0000) = ch;//send char
	while(!((*(volatile int*)(SERIAL_ADDR + 0x0014) & 0x60) == 0x60));//wait for sending done
}
void dbg_uart_init(void)
{
	*(volatile int*)0xB0010018 = 1 << 31;
	*(volatile int*)0xB0010028 = 1 << 31;
	*(volatile int*)0xB0010038 = 1 << 31;
	*(volatile int*)0xB0010044 = 1 << 31;
	*(volatile int*)(REG_CLKGR) &= ~(1<<(15+SERIAL_IDX));//enable uart clock
	*(volatile int*)(SERIAL_ADDR + 0x0004) = 0;//disable serial interrupts
	*(volatile int*)(SERIAL_ADDR + 0x0004) = 0;//disable serial interrupts
	*(volatile int*)(SERIAL_ADDR + 0x0008) = 0;//disable uart
	*(volatile int*)(SERIAL_ADDR + 0x0020) = ~((1<<0)|(1<<1));//set tx rx on UART mode
	*(volatile int*)(SERIAL_ADDR + 0x000C) = (3<<0)|(0<<2);
	//set baud_rate
#define BAUD_DIV ((SYS_EXTAL)/16/(SERIAL_BAUD)-1)
	*(volatile int*)(SERIAL_ADDR + 0x000C) |= (1<<7);//enable setting uart clk_div
	*(volatile int*)(SERIAL_ADDR + 0x0000) = (BAUD_DIV) & 0xFF;
	*(volatile int*)(SERIAL_ADDR + 0x0004) = ((BAUD_DIV)>>8) & 0xFF;
	*(volatile int*)(SERIAL_ADDR + 0x000C) &= ~(1<<7);//disable setting uart clk_div
	//set fifo mode & enable uart
	*(volatile int*)(SERIAL_ADDR + 0x0008) = (1<<0)|(1<<1)|(1<<2)|(1<<4);
}
void put_char(char chn, char ch)
{
	if(chn==0)
		dbg_uart_putc(ch);
	else
		gpio_uart_putc(ch);
}
void put_str(char chn, char *str)
{
	while(*str)
		put_char(chn, *str++);
}

void put_addr(char chn, int num)
{
	char i;
	unsigned char ch[8];
	put_char(chn, '0');
	put_char(chn, 'x');
	for(i=0;i<8;i++){
		ch[i] = num & 0x0F;
		num >>= 4;
	}
	for(i=7;i>=0;i--){
		if(ch[i]>=10)
			put_char(chn, ch[i]-10+'A');
		else
			put_char(chn, ch[i]+'0');
	}
}
