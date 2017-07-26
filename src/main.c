#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fbs_patchram.h"

#define FBS_ENV_DEV_PATH "FBS_ENV_DEV_PATH"
#define FBS_ENV_FW_PATH  "FBS_ENV_FW_PATH"

extern void* fbs_command_recv(void *p);
extern void* fbs_hci_recv(void *p);
extern void fbs_stack_init_all(void);

int main(int argc, char *argv[])
{
	int res;
	tFBS_patchram info;
	g_setenv(FBS_ENV_DEV_PATH,
			"/dev/ttyS0", FALSE);
	g_setenv(FBS_ENV_FW_PATH,
			"/lib/firmware/BCM4343A0_001.001.034.0056.0221_26M_ORC.hcd", FALSE);
#if 1
	info.type = FBS_PATCHRAM_TYPE_UART;
	system("echo 1 > /sys/class/rfkill/rfkill0/state");//Power on BT chip
	strcpy(info.uart.dev_path, g_getenv(FBS_ENV_DEV_PATH));
	strcpy(info.uart.fw_path, g_getenv(FBS_ENV_FW_PATH));
	if((res = FBS_patchram(&info)) < 0){
		g_error("Init bluetooth patchram failed : %d", res);
	}
#endif
	pthread_t th_hci_recv;
	pthread_t th_command_recv;
	GMainLoop *mainloop = NULL;
	g_setenv("G_MESSAGES_DEBUG", "all", TRUE);
	FBS_stack_init_all();
	mainloop = g_main_loop_new (NULL, FALSE);
	pthread_create(&th_hci_recv, NULL, fbs_hci_recv, mainloop);
	pthread_create(&th_command_recv, NULL, fbs_command_recv, mainloop);
	g_main_loop_run(mainloop);
	g_main_loop_unref(mainloop);
	pthread_join(th_hci_recv, NULL);
	pthread_join(th_command_recv, NULL);
	return 0;
}
