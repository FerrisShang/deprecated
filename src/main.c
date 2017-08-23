#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "fbs_patchram.h"
#include "fbs_stack.h"

#include <gio/gio.h>
int main(int argc, char *argv[])
{
	g_setenv(FBS_ENV_DEV_PATH,
			"/dev/ttyS0", FALSE);
	g_setenv(FBS_ENV_FW_PATH,
			"/lib/firmware/BCM4343A0_001.001.034.0056.0221_26M_ORC.hcd", FALSE);
	g_setenv(FBS_BTSNOOP_PATH,
			"/tmp/btsnoop_hci.log" , FALSE);
#if 1
	int res;
	tFBS_patchram info;
	info.type = FBS_PATCHRAM_TYPE_UART;
	system("if cat /sys/class/rfkill/rfkill0/state | grep -q \"1\" ;"
			"then echo 0 > /sys/class/rfkill/rfkill0/state ;fi");//Close BT if already open
	system("echo 1 > /sys/class/rfkill/rfkill0/state");//Power on BT chip
	strcpy(info.uart.dev_path, g_getenv(FBS_ENV_DEV_PATH));
	strcpy(info.uart.fw_path, g_getenv(FBS_ENV_FW_PATH));
	if((res = FBS_patchram(&info)) < 0){
		g_error("Init bluetooth patchram failed : %d", res);
	}
#else
	g_setenv(FBS_ENV_DEV_PATH,
			"/dev/ttyUSB0", TRUE);
	int speed = B3000000;
	struct termios tty;
	const gchar *dev_path = g_getenv(FBS_ENV_DEV_PATH);
	if(!dev_path){
		g_error("Can not find device path");
	}
	int uart_fd  = open (dev_path, O_RDWR | O_NOCTTY | O_SYNC);
	if (uart_fd < 0) {
		g_error("Can not open uart : %s", dev_path);
	}
	memset (&tty, 0, sizeof tty);
	tcgetattr(uart_fd, &tty);
	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);
	tty.c_cflag &= ~CRTSCTS;
	tcflush(uart_fd, TCIOFLUSH);
	tcsetattr (uart_fd, TCSANOW, &tty);
	tcflush(uart_fd, TCIOFLUSH);
	close(uart_fd);
#endif
	g_setenv("G_MESSAGES_DEBUG", "all", TRUE);
	FBS_stack_init_all();
	FBS_stack_run();
	FBS_stack_destroy_all();
	g_message("Exit success");
	return 0;
}
