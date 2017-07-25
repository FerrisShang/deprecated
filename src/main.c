#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern void* fbs_command_recv(void *p);
extern void* fbs_hci_recv(void *p);
extern void fbs_stack_init_all(void);

int main(int argc, char *argv[])
{
	pthread_t th_hci_recv;
	pthread_t th_command_recv;
	GMainLoop *mainloop = NULL;
	g_setenv("G_MESSAGES_DEBUG", "all", TRUE);
	fbs_stack_init_all();
	mainloop = g_main_loop_new (NULL, FALSE);
	pthread_create(&th_hci_recv, NULL, fbs_hci_recv, mainloop);
	pthread_create(&th_command_recv, NULL, fbs_command_recv, mainloop);
	g_main_loop_run(mainloop);
	g_main_loop_unref(mainloop);
	pthread_join(th_hci_recv, NULL);
	pthread_join(th_command_recv, NULL);
	return 0;
}
