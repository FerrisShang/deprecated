#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "fbs_stack.h"
#include "fbs_patchram.h"
#include "fbs_uart.h"
#include "fbs_hci.h"
#include "fbs_l2cap.h"

static void* fbs_command_recv(void *p); //RFU

struct FBS_stack_info {
	gint (*uart_send)(guchar *data, gint len);
};

static struct FBS_stack_info FBS_stack_info;

static void fbs_stack_hci_cb(guint8 evt, guint8 *data, guint16 len)
{
	int i;
	for(i=0;i<len;i++){
		if(!(i%16)){printf("\n");}
		printf("%02x ", data[i]);
	} printf("\n");
}

void FBS_stack_init_all(void)
{
	const gchar *btsnoop_path;
	btsnoop_path = g_getenv(FBS_BTSNOOP_PATH);
	if(btsnoop_path != NULL){
		FBS_enable_btsnoop(btsnoop_path);
	}

	FBS_stack_info.uart_send = FBS_uart_send;
	FBS_hci_init(fbs_stack_hci_cb);
}

void FBS_uart_report(guchar *data, gint len, gpointer unused)
{
	switch(FBS_UART_MSG_TYPE(data)){
		case FBS_UART_ACLDATA_PKT :
			FBS_l2cap_data_process(data, len);
			break;
		case FBS_UART_EVENT_PKT :
			FBS_hci_evt_process(data, len);
			break;
		default :
			g_warning("Unsupport UART message, type : %d", FBS_UART_MSG_TYPE(data));
			break;
	}
}

void FBS_stack_run(void)
{
	pthread_t th_uart_recv;
	pthread_t th_command_recv;
	GMainLoop *mainloop = NULL;
	mainloop = g_main_loop_new (NULL, FALSE);
	FBS_uart_init(mainloop, FBS_uart_report, NULL);
	pthread_create(&th_uart_recv, NULL, FBS_uart_recv, NULL);
	g_usleep(5000);//wait for thread running
	pthread_create(&th_command_recv, NULL, fbs_command_recv, mainloop);
	g_usleep(5000);//wait for thread running
	g_main_loop_run(mainloop);
	g_main_loop_unref(mainloop);
	pthread_join(th_uart_recv, NULL);
	pthread_join(th_command_recv, NULL);
}

#define FBS_RESERVE_BUF 4 // [len(2) + rfu(2)] + uart data
#define FBS_UART_DATA_POINTER(p) (&(((guchar*)p)[FBS_RESERVE_BUF]))
#define FBS_UART_DATA_LEN(p)     ((guchar)p[0]+ ((guchar)p[1]<<8))

static gboolean fbs_uart_send_cb(gpointer data)
{
	guchar *p = data;
	FBS_stack_info.uart_send(FBS_UART_DATA_POINTER(p), FBS_UART_DATA_LEN(p));
	return FALSE;
}

static void fbs_uart_send_destroy_cb(gpointer data)
{
	guchar *p = data;
	g_slice_free1(FBS_UART_DATA_LEN(p)+FBS_RESERVE_BUF, p);
}

void FBS_hci_send_raw(guchar *data, guint len)
{
	guchar *hci_data = g_slice_alloc(sizeof(len + FBS_RESERVE_BUF));
	memcpy(hci_data, data, sizeof(len));
	hci_data[0] = len & 0xFF; //len
	hci_data[1] = (len >> 8) & 0xFF;
	g_main_context_invoke_full(
			NULL,
			G_PRIORITY_DEFAULT,
			fbs_uart_send_cb,
			hci_data,
			fbs_uart_send_destroy_cb);
}


static void* fbs_command_recv(void *p) //RFU
{
	GMainLoop *mainloop = p;
	while(g_main_loop_is_running(mainloop) == FALSE){ g_usleep(1000); }
	while(TRUE){
		if(g_main_loop_is_running(mainloop) == FALSE){ break; }
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_HOST_CTL, OCF_READ_LOCAL_NAME), NULL, 0);
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_INFO_PARAM, OCF_READ_BUFFER_SIZE), NULL, 0);
		tFBS_host_buffer_size_cp host_buffer_size_cp;
		FBS_get_acl_mtu(&host_buffer_size_cp.acl_mtu, &host_buffer_size_cp.acl_max_pkt);
		FBS_get_sco_mtu(&host_buffer_size_cp.sco_mtu, &host_buffer_size_cp.sco_max_pkt);
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_HOST_CTL, OCF_HOST_BUFFER_SIZE),
				(gpointer)&host_buffer_size_cp, sizeof(tFBS_host_buffer_size_cp));
		g_message("Done");
		g_usleep(~0);
	}
	return NULL;
}
