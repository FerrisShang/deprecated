#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fbs_patchram.h"
#include "fbs_stack.h"

#include <gio/gio.h>
int main(int argc, char *argv[])
{
	int res;
	g_setenv(FBS_ENV_DEV_PATH,
			"/dev/ttyS0", FALSE);
	g_setenv(FBS_ENV_FW_PATH,
			"/lib/firmware/BCM4343A0_001.001.034.0056.0221_26M_ORC.hcd", FALSE);
	g_setenv(FBS_BTSNOOP_PATH,
			"/tmp/btsnoop_hci.log" , FALSE);
#if 1
	tFBS_patchram info;
	info.type = FBS_PATCHRAM_TYPE_UART;
	system("echo 1 > /sys/class/rfkill/rfkill0/state");//Power on BT chip
	strcpy(info.uart.dev_path, g_getenv(FBS_ENV_DEV_PATH));
	strcpy(info.uart.fw_path, g_getenv(FBS_ENV_FW_PATH));
	if((res = FBS_patchram(&info)) < 0){
		g_error("Init bluetooth patchram failed : %d", res);
	}
#endif
	g_setenv("G_MESSAGES_DEBUG", "all", TRUE);
	FBS_stack_init_all();
	FBS_stack_run();
	return 0;
}
