#include <stdlib.h>
#include <stdio.h>

void dbg_uart_init(void);
void dbg_uart_putc(char ch);
void put_str(char chn, char *str);
void put_char(char chn, char ch);
void put_addr(char chn, int num);

int reg_addr[] = {
	0xb0000000, 0xb00000D4, 0xb000002C, 0xb0000030, 0xb0000060, 0xb00000A0,
	0xb0000050, 0xb0000064, 0xb0000068, 0xb00000A4, 0xb00000A8, 0xb000006C,
	0xb0000074, 0xb000007C, 0xb0000080, 0xb0000084, 0xb00000AC, 0xb00000B0,
	0xb00000B4, 0xb0000034, 0xb0000038, 0xb000003C, 0xb0000040, 0xb0000044,
	0xb0000048, 0xb000000C, 0xb0000010, 0xb0000014, 0xb0000018, 0xb000001C,
	0xb0000004, 0xb00000B8, 0xb00000BC, 0xb0000090, 0xb0000094, 0xb0000098,
	0xb000009c, 0xb0000020, 0xb0000028, 0xb00000C4, 0xb00000C8, 0xb00000CC,
	0xb00000D8, 0xb00000DC, 0xb0000024,
};
void main(void)
{
	int buf[80];
	int i, tmp;
	gpio_uart_init();
	dbg_uart_init();
	put_str(1, "gpio uart init ok.\r\n");
	//put_str(0, "dbg uart init ok.\n");

	put_addr(1,*(volatile int*)0xB0000020);
	put_str(1, "\r\n");
	put_addr(1,*(volatile int*)0xB0033004);
	put_str(1, "\r\n");
	put_addr(1,*(volatile int*)0xB0033008);
	put_str(1, "\r\n");
	put_addr(1,*(volatile int*)0xB0033020);
	put_str(1, "\r\n");
	put_addr(1,*(volatile int*)0xB003300C);
	put_str(1, "\r\n");
#if 0
	*(volatile int*)0xB0000020 &= ~(1<<18);//enable uart3 clock
	*(volatile int*)0xB003300C = 0x00000003;//set uart3 format
	*(volatile int*)0xB0033004 = 250;//set uart3 baud rate
	*(volatile int*)0xB0033008 = 1<<4;//enable uart3
	while(*(volatile int*)0xB0033014 & (1<<6)){
		*(volatile int*)0xB0033000 = 0x30+((i++)%10);
	}
	put_addr(*(volatile int*)0xB0000020);
	put_addr(*(volatile int*)0xB32B0000);
	put_str("\n\r");
	led_init();
	put_str("\n\rconfig0~config3:\n\r");
	getconfig();
	put_str("\n\rstatus:\n\r");
	getstatus();
	//register read test
	for(i=0;i<45;i++){
		put_addr(reg_addr[i]);
		put_str(" = ");
		put_addr(*(volatile int*)reg_addr[i]);
		put_str("\n\r");
	}
#endif
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
	while(!(*(volatile int*)(SERIAL_ADDR + 0x0004) & 0x60 == 0x60));
}
void dbg_uart_init(void)
{
	//config uart pin function
	*(volatile int*)0xB0010018 = 1 << 31;
	*(volatile int*)0xB0010028 = 1 << 31;
	*(volatile int*)0xB0010038 = 1 << 31;
	*(volatile int*)0xB0010044 = 1 << 31;

	*(volatile int*)(REG_CLKGR) &= ~(1<<(15+SERIAL_IDX));//enable uart clock
	*(volatile int*)(SERIAL_ADDR + 0x0004) = 0;//disable serial interrupts
	*(volatile int*)(SERIAL_ADDR + 0x0004) = 0;//disable serial interrupts
	*(volatile int*)(SERIAL_ADDR + 0x0008) = 0;//disable uart
	*(volatile int*)(SERIAL_ADDR + 0x0020) = ~((1<<0)|(1<<1));//set tx rx on UART mode
	*(volatile int*)(SERIAL_ADDR + 0x000C) = (3<<0)|(0<<2);//set bit width & parity
	//set baud_rate
#define BAUD_DIV ((SYS_EXTAL)/16/(SERIAL_BAUD)-1)
	*(volatile int*)(SERIAL_ADDR + 0x000C) |= (1<<7);//enable setting uart clk_div
	*(volatile int*)(SERIAL_ADDR + 0x0000) = ((BAUD_DIV)>>8) & 0xFF;
	*(volatile int*)(SERIAL_ADDR + 0x0004) = (BAUD_DIV) & 0xFF;
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
