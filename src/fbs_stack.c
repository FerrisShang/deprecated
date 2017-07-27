#include <stdio.h>
#include <glib.h>
#include "fbs_stack.h"
#include "fbs_uart.h"

struct FBS_stack_info {
	guint (*hci_send)(guchar *data, guint len);
};

static struct FBS_stack_info FBS_stack_info;
void FBS_stack_init_all(void)
{
	const gchar *btsnoop_path;
	btsnoop_path = g_getenv(FBS_BTSNOOP_PATH);
	if(btsnoop_path != NULL){
		FBS_enable_btsnoop(btsnoop_path);
	}

	FBS_stack_info.hci_send = FBS_uart_send;
}

void FBS_hci_report(guchar *user_data, guint len)
{
	int i;
	for(i=0;i<len;i++){
		if(!(i%16)) printf("\n");
		printf("%02x ", user_data[i]);
	}
	printf("\n");
	puts((char*)&user_data[7]);
}

void FBS_stack_run(void)
{
	pthread_t th_hci_recv;
	pthread_t th_command_recv;
	GMainLoop *mainloop = NULL;
	mainloop = g_main_loop_new (NULL, FALSE);
	pthread_create(&th_hci_recv, NULL, (void*(*)(void*))FBS_uart_recv, mainloop);
	pthread_create(&th_command_recv, NULL, fbs_command_recv, mainloop);
	g_main_loop_run(mainloop);
	g_main_loop_unref(mainloop);
	pthread_join(th_hci_recv, NULL);
	pthread_join(th_command_recv, NULL);
}

void FBS_hci_send_raw(guchar *data, guint len)
{
	FBS_stack_info.hci_send(data, len);
}
