#include <regdef.h>
#define DELAY_TIME 78
	.set noreorder
	.text
	.word 0x55020304
	.word 0x00aa55aa
	.word 0
	.word 0
	.globl _start
_start:
	jal		main
	nop
	b		.
	nop

.type gpio_uart_init, @function
.globl gpio_uart_init
gpio_uart_init:
#define UART_PORT_BASE	 0xb0010500
#define UART_PIN		 (1<<4)
	la      v0, UART_PORT_BASE
	li      t0, UART_PIN
	sw      t0, 0x74(v0)
	sw      t0, 0x18(v0)
	sw      t0, 0x24(v0)
	sw      t0, 0x38(v0)
	sw      t0, 0x44(v0)
	jr		ra
	nop 

.type gpio_uart_putc, @function
.globl gpio_uart_putc
gpio_uart_putc:
	la      v0, UART_PORT_BASE
	li      t0, UART_PIN
	sw      t0, 0x48(v0)
	li      t1, DELAY_TIME
1:
	subu    t1, t1, 1
	nop 
	bgtz    t1, 1b
	nop 
	li      t2, 8
loop:
	subu    t2, t2, 1
	and		t3, a0, 1 
	bgtz    t3, 2f
	nop 
	sw      t0, 0x48(v0)
	j 3f
	nop 
2:
	sw      t0, 0x44(v0)
3:
	li      t1, DELAY_TIME
1:
	subu    t1, t1, 1
	nop 
	bgtz    t1, 1b
	nop 
	srl		a0, a0, 1
	bgtz    t2, loop
	nop 
	sw      t0, 0x44(v0)
	li      t1, DELAY_TIME
1:
	subu    t1, t1, 1
	nop 
	bgtz    t1, 1b
	nop 
	jr		ra
	nop 
