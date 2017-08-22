#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <glib.h>
#include "fbs_stack.h"
#include "fbs_hci.h"
#include "fbs_ble.h"
#include "fbs_acl.h"
#include "fbs_l2cap.h"
#include "fbs_uart.h"

struct fbs_hci_tcb {
	GMutex mutex;
	sem_t sem_cmd_num;
	GSList *evt_cb_list; //tFBS_hci_cb
	guint hci_tout_evt_id;
};

static struct fbs_hci_tcb *fbs_hci_tcb;

static struct fbs_hci_tcb* FBS_create_hci_tcb(void)
{
	struct fbs_hci_tcb *tcb;
	tcb = g_new0(struct fbs_hci_tcb, 1);
	g_mutex_init(&tcb->mutex);
	sem_init(&tcb->sem_cmd_num, 0, 1);
	tcb->evt_cb_list = NULL;
	return tcb;
}

void FBS_hci_reg_callback(tFBS_hci_cb callback)
{
	if(fbs_hci_tcb == NULL){
		fbs_hci_tcb = FBS_create_hci_tcb();
	}
	fbs_hci_tcb->evt_cb_list = g_slist_prepend(fbs_hci_tcb->evt_cb_list, callback);
}

void FBS_hci_destroy(void)
{
	g_assert(fbs_hci_tcb != NULL);
	sem_destroy(&fbs_hci_tcb->sem_cmd_num);
	g_mutex_clear(&fbs_hci_tcb->mutex);
	g_free(fbs_hci_tcb);
}

static gboolean fbs_hci_tout_cb(gpointer user_data)
{
	guint16 *op = (guint16*)user_data;
	g_warning("HCI Command Timeout ! OGF:0x%02x OCF:0x%02x",
			FBS_CMD_OPCODE_OGF(*op), FBS_CMD_OPCODE_OCF(*op));
	return G_SOURCE_REMOVE;
}
static void fbs_hci_tout_destroy(gpointer data)
{
	g_slice_free(guint16, data);
}
static void fbs_add_hci_tout(guint time_second, guint16 opcode)
{
	guint16 *op = g_slice_new(guint16);
	*op = opcode;
	fbs_hci_tcb->hci_tout_evt_id = g_timeout_add_seconds_full(
			G_PRIORITY_DEFAULT,
			time_second,
			fbs_hci_tout_cb,
			op,
			fbs_hci_tout_destroy);
}
static void fbs_del_hci_tout(void)
{
	g_source_remove(fbs_hci_tcb->hci_tout_evt_id);
}

static void fbs_evt_callback(gpointer data, gpointer user_data)
{
	tFBS_evt_header *header = (tFBS_evt_header*)user_data;
	tFBS_hci_cb callback = data;
	if(header->type == FBS_UART_RESERVE){ /* FBS_UART_RESERVE means event processed*/
		return;
	}
	if(callback(header->evt_code, header->data, header->evt_len) == TRUE){
		header->type = FBS_UART_RESERVE;
	}
}
void FBS_hci_evt_process(guchar *data, gint len)
{
	g_assert(fbs_hci_tcb != NULL);
	guchar allow_num = 0;
	tFBS_evt_header *header = (tFBS_evt_header*)data;
	if(header->evt_code == EVT_CMD_COMPLETE){
		tFBS_evt_cmd_complete *p = (tFBS_evt_cmd_complete*)header->data;
		allow_num = p->ncmd;
		fbs_del_hci_tout();
	}else if(header->evt_code == EVT_CMD_STATUS){
		tFBS_evt_cmd_status *p = (tFBS_evt_cmd_status*)header->data;
		allow_num = p->ncmd;
		fbs_del_hci_tout();
	}else if(header->evt_code == EVT_NUM_COMP_PKTS){
		guint8 i;
		tFBS_evt_num_comp_pkts *p = (tFBS_evt_num_comp_pkts*)header->data;
		for(i=0;i<p->num_hndl;i++){
			guint16 handle   = p->data[0+i*4]+(p->data[1+i*4]<<8);
			guint16 com_pack = p->data[2+i*4]+(p->data[3+i*4]<<8);
			FBS_acl_comp_packets(handle, com_pack);
		}
	}
	if(allow_num > 0){
		int i;
		for(i=0;i<allow_num;i++){
			sem_post(&fbs_hci_tcb->sem_cmd_num);
		}
	}
	g_slist_foreach(fbs_hci_tcb->evt_cb_list, fbs_evt_callback, header);
	if(header->type != FBS_UART_RESERVE){ /* FBS_UART_RESERVE means event processed*/
		g_warning("Unprocessed HCI event message, type : 0x%02x", header->evt_code);
	}
}

#define FBS_RESERVE_BUF_LEN 4 // [len(2) + rfu(2)] + uart data
#define FBS_UART_DATA_POINTER(p) (&(((guchar*)p)[FBS_RESERVE_BUF_LEN]))
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
	g_slice_free1(FBS_UART_DATA_LEN(p)+FBS_RESERVE_BUF_LEN, p);
}

void FBS_hci_send(guint16 opcode, gpointer *data, gint len)
{
	g_assert(fbs_hci_tcb != NULL);
	sem_wait(&fbs_hci_tcb->sem_cmd_num);
	guchar *hci_data = g_slice_alloc(FBS_RESERVE_BUF_LEN+4+len);
	hci_data[0] = ((4 + len) >> 0) & 0xFF;
	hci_data[1] = ((4 + len) >> 8) & 0xFF;
	hci_data[FBS_RESERVE_BUF_LEN+0] = 1; //cmd
	hci_data[FBS_RESERVE_BUF_LEN+1] = (opcode >> 0) & 0xFF;
	hci_data[FBS_RESERVE_BUF_LEN+2] = (opcode >> 8) & 0xFF;
	hci_data[FBS_RESERVE_BUF_LEN+3] = (guchar)len;
	memcpy(&hci_data[FBS_RESERVE_BUF_LEN+4], data, len);
	fbs_add_hci_tout(1, opcode);
	g_main_context_invoke_full(
			NULL,
			G_PRIORITY_DEFAULT,
			fbs_uart_send_cb,
			hci_data,
			fbs_uart_send_destroy_cb);
}
