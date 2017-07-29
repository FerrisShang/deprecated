#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <glib.h>
#include "fbs_stack.h"
#include "fbs_hci.h"
#include "fbs_hci_type.h"
#include "fbs_l2cap.h"
#include "fbs_uart.h"

struct fbs_hci_tbc {
	GMutex mutex;
	sem_t sem_cmd_num;
	tFBS_hci_cb evt_callback;
};

static struct fbs_hci_tbc fbs_hci_tbc;

void FBS_hci_init(tFBS_hci_cb callback)
{
	g_assert(callback != NULL);
	memset(&fbs_hci_tbc, 0, sizeof(struct fbs_hci_tbc));
	g_mutex_init(&fbs_hci_tbc.mutex);
	sem_init(&fbs_hci_tbc.sem_cmd_num, 0, 1);
	fbs_hci_tbc.evt_callback = callback;
}

void FBS_hci_destroy(void)
{
	sem_destroy(&fbs_hci_tbc.sem_cmd_num);
	g_mutex_clear(&fbs_hci_tbc.mutex);
}

void FBS_hci_evt_process(guchar *data, gint len)
{
	guchar allow_num = 0;
	tFBS_evt_header *header = (tFBS_evt_header*)data;
	if(header->evt_code == EVT_CMD_COMPLETE){
		tFBS_evt_cmd_complete *p = (tFBS_evt_cmd_complete*)header->data;
		allow_num = p->ncmd;
	}else if(header->evt_code == EVT_CMD_STATUS){
		tFBS_evt_cmd_status *p = (tFBS_evt_cmd_status*)header->data;
		allow_num = p->ncmd;
	}else if(header->evt_code == EVT_NUM_COMP_PKTS){
		guint8 i;
		tFBS_evt_num_comp_pkts *p = (tFBS_evt_num_comp_pkts*)header->data;
		for(i=0;i<p->num_hndl;i++){
			guint16 handle   = p->data[0+i*4]+(p->data[1+i*4]<<8);
			guint16 com_pack = p->data[2+i*4]+(p->data[3+i*4]<<8);
			FBS_l2cap_comp_packets(handle, com_pack);
		}
	}
	if(allow_num > 0){
		int i;
		for(i=0;i<allow_num;i++){
			sem_post(&fbs_hci_tbc.sem_cmd_num);
		}
	}
	fbs_hci_tbc.evt_callback(header->evt_code, header->data, header->evt_len);
}

#define FBS_RESERVE_BUF 4 // [len(2) + rfu(2)] + uart data
#define FBS_UART_DATA_POINTER(p) (&(((guchar*)p)[FBS_RESERVE_BUF]))
#define FBS_UART_DATA_LEN(p)     ((guchar)p[0]+ ((guchar)p[1]<<8))

static gboolean fbs_uart_send_cb(gpointer data)
{
	guchar *p = data;
	FBS_uart_send(FBS_UART_DATA_POINTER(p), FBS_UART_DATA_LEN(p));
	return FALSE;
}

static void fbs_uart_send_destroy_cb(gpointer data)
{
	guchar *p = data;
	g_slice_free1(FBS_UART_DATA_LEN(p)+FBS_RESERVE_BUF, p);
}

void FBS_hci_send(guint16 opcode, gpointer *data, gint len)
{
	sem_wait(&fbs_hci_tbc.sem_cmd_num);
	guchar *hci_data = g_slice_alloc(FBS_RESERVE_BUF+4+len);
	hci_data[0] = ((4 + len) >> 0) & 0xFF;
	hci_data[1] = ((4 + len) >> 8) & 0xFF;
	hci_data[FBS_RESERVE_BUF+0] = 1; //cmd
	hci_data[FBS_RESERVE_BUF+1] = (opcode >> 0) & 0xFF;
	hci_data[FBS_RESERVE_BUF+2] = (opcode >> 8) & 0xFF;
	hci_data[FBS_RESERVE_BUF+3] = (guchar)len;
	memcpy(&hci_data[FBS_RESERVE_BUF+4], data, len);

	g_main_context_invoke_full(
			NULL,
			G_PRIORITY_DEFAULT,
			fbs_uart_send_cb,
			hci_data,
			fbs_uart_send_destroy_cb);
}
