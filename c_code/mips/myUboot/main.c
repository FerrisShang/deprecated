#include <stdlib.h>
#include <stdio.h>
#define REG(addr) *(volatile int*)(addr)
void dbg_uart_init(void);
void dbg_uart_putc(char ch);
void put_str(char chn, char *str);
void put_char(char chn, char ch);
void put_addr(char chn, int num);
void init_ddr(void);

void main(void)
{
	int buf[80];
	int i, tmp;
	gpio_uart_init();
	dbg_uart_init();
	put_str(1, "gpio uart init ok.\r\n");
	put_str(0, "dbg uart init ok.\n");
	init_ddr();
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
	REG(SERIAL_ADDR + 0x0000) = ch;//send char
	while(!((REG(SERIAL_ADDR + 0x0014) & 0x60) == 0x60));//wait for sending done
}
void dbg_uart_init(void)
{
	REG(0xB0010018) = 1 << 31;
	REG(0xB0010028) = 1 << 31;
	REG(0xB0010038) = 1 << 31;
	REG(0xB0010044) = 1 << 31;
	REG(REG_CLKGR) &= ~(1<<(15+SERIAL_IDX));//enable uart clock
	REG(SERIAL_ADDR + 0x0004) = 0;//disable serial interrupts
	REG(SERIAL_ADDR + 0x0008) = 0;//disable uart
	REG(SERIAL_ADDR + 0x0020) = ~((1<<0)|(1<<1));//set tx rx on UART mode
	REG(SERIAL_ADDR + 0x000C) = (3<<0)|(0<<2);//set data format
	//set baud_rate
#define BAUD_DIV ((SYS_EXTAL)/16/(SERIAL_BAUD)-1)
	REG(SERIAL_ADDR + 0x000C) |= (1<<7);//enable setting uart clk_div
	REG(SERIAL_ADDR + 0x0000) = (BAUD_DIV) & 0xFF;
	REG(SERIAL_ADDR + 0x0004) = ((BAUD_DIV)>>8) & 0xFF;
	REG(SERIAL_ADDR + 0x000C) &= ~(1<<7);//disable setting uart clk_div
	//set fifo mode & enable uart
	REG(SERIAL_ADDR + 0x0008) = (1<<0)|(1<<1)|(1<<2)|(1<<4);
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

void init_ddr(void)
{
#define DDR_RATE 200000000
#define DDRC_BASE 0xB3010000
	REG(0xB00000D0) = 0x11;//reset DLL
	//mdelay(5);
	REG(DDRC_BASE+0x08) = 0x0F << 20;//reset DDR Controller
	//mdelay(5);
	REG(DDRC_BASE+0x08) = 0x00;//reset DDR Controller
	//mdelay(5);
	/*
	 * set REG_DCFG
	 * 16-bit row address
	 * 8-bit Column address
	 * 4 bank device
	 * IMBA = 1
	 * BSL = 0; 4 burst
	 * TYPE = Mobile DDR(LPDDR)
	 * CS1_EN = 0;CS0_EN = 1;
	 * DW = 1;External memory data width is 32-bit
	 */
	REG(DDRC_BASE+0x04) = 0x20468a69;
	REG(DDRC_BASE+0x08) = 1<<1; //force CKE1 HIGH
	REG(DDRC_BASE+0x1000+0x54) = 0x00150000;//Data Training Address Register
	REG(DDRC_BASE+0x1000+0x30) = 0x00000000;//DCR
	REG(DDRC_BASE+0x1000+0x40) = 0x00000033;//MR0
	REG(DDRC_BASE+0x1000+0x18) = 0x00208005;//PRT0
	REG(DDRC_BASE+0x1000+0x1C) = 0x00504E20;//PRT1
	REG(DDRC_BASE+0x1000+0x20) = 0x0014000A;//PRT2
	REG(DDRC_BASE+0x1000+0x20) = 0x8C45222A;//DTPR0
	REG(DDRC_BASE+0x1000+0x20) = 0x00080028;//DTPR1
	REG(DDRC_BASE+0x1000+0x20) = 0x1000080C;//DTPR2
	REG(DDRC_BASE+0x1000+0x1c0+0*0x40) &= ~(3<<9);//DX1GCR
	REG(DDRC_BASE+0x1000+0x1c0+1*0x40) &= ~(3<<9);//DX2GCR
	REG(DDRC_BASE+0x1000+0x1c0+2*0x40) &= ~(3<<9);//DX3GCR
	REG(DDRC_BASE+0x1000+0x1c0+3*0x40) &= ~(3<<9);//DX4GCR
	REG(DDRC_BASE+0x1000+0x08) = 0x01042e03; //PGCR
	REG(DDRC_BASE+0x1000+0x24) = 0x30c00813; //ACIOCR
	REG(DDRC_BASE+0x1000+0x28) = 0x4802; //DXCCR
	int timeout = 100000;
	while (!(REG(DDRC_BASE+0x1000+0xc) == 0x07)//check PGSR
			&& (REG(DDRC_BASE+0x1000+0xc) != 0x1F)
			&& --timeout);
	if (timeout == 0) {
		put_str(0, "DDR PHY init timeout.\n"); while(1);
	}else{
		put_str(0, "DDR PHY init done.\n");
	}

	REG(DDRC_BASE+0x1000+0x4) = (1<<0)|(1<<5)|(1<<6)|(1<<17); //DRP_PIR
	timeout = 100000;
	while (!(REG(DDRC_BASE+0x1000+0xc) == 0x0F)//check PGSR
			&& (REG(DDRC_BASE+0x1000+0xc) != 0x1F)
			&& --timeout);
	if (timeout == 0) {
		put_str(0, "DDR init timeout.\n"); while(1);
	}else{
		put_str(0, "DDR init done.\n");
	}
	//DDR hard Training
	REG(DDRC_BASE+0x1000+0x4) = (1<<0)|(1<<2)|(1<<7); //DRP_PIR
	timeout = 50;
	while ((REG(DDRC_BASE+0x1000+0xc) != 0x1F)//check PGSR
			&& !(REG(DDRC_BASE+0x1000+0xc) & (7<<4))
			&& --timeout);
	if(timeout == 0){
		put_str(0, "DDR training timeout\n");
	}else if(REG(DDRC_BASE+0x1000+0xc)&((1<<5)|(1<<6))){
		put_str(0, "DDR hardware training error\n");
	}else{
		put_str(0, "DDR hardware training done\n");
	}
}
