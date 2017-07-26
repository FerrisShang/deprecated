#include <glib.h>

void* fbs_hci_recv(void *p)
{
	GMainLoop *mainloop = p;
	while(g_main_loop_is_running(mainloop) == FALSE){ g_usleep(1000); }
	while(TRUE){
		if(g_main_loop_is_running(mainloop) == FALSE){ break; }
		g_usleep(1000000);
		g_info("%s@%d\n", __func__, __LINE__);
	}
	return NULL;
}

void* fbs_command_recv(void *p) //RFU
{
	GMainLoop *mainloop = p;
	while(g_main_loop_is_running(mainloop) == FALSE){ g_usleep(1000); }
	while(TRUE){
		if(g_main_loop_is_running(mainloop) == FALSE){ break; }
		g_usleep(~0);
	}
	return NULL;
}
