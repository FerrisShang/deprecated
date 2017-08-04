#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <semaphore.h>
#include "fbs_l2cap.h"
#include "fbs_stack.h"
#include "fbs_uart.h"


struct fbs_l2cap_link_tcb {
	struct fbs_l2cap_link *conn;
};

struct fbs_l2cap_tcb {
	GSList *link_list; //struct fbs_l2cap_link_tcb
};

struct fbs_l2cap_header {
	guint16 len;
	guint16 cid;
	guint8 data[0];
} __attribute__ ((packed)) tFBS_acl_header;

static struct fbs_l2cap_tcb *fbs_l2cap_tcb;

void FBS_l2cap_init(void)
{
	if(fbs_l2cap_tcb == NULL){
		fbs_l2cap_tcb = g_new0(struct fbs_l2cap_tcb, 1);
		fbs_l2cap_tcb->link_list = NULL;
	}
}
static void fbs_destroy_list(gpointer data)
{
	struct fbs_l2cap_link_tcb *link_tcb = (struct fbs_l2cap_link_tcb*) data;
	g_free(link_tcb->conn);
	g_free(link_tcb);
}
void FBS_l2cap_destroy(void)
{
	if(fbs_l2cap_tcb != NULL){
		if(fbs_l2cap_tcb->link_list != NULL){
			g_slist_free_full(fbs_l2cap_tcb->link_list, fbs_destroy_list);
		}
		g_free(fbs_l2cap_tcb);
	}
}

void FBS_l2cap_data_process(guchar *data, gint len)
{
	struct fbs_l2cap_header *p = (struct fbs_l2cap_header*)data;
	switch(p->cid){
		case FBS_L2CAP_CID_L2CAP:
			g_warning("NOT finished");
			break;
		case FBS_L2CAP_CID_CONNLESS:
			g_warning("NOT finished");
			break;
		case FBS_L2CAP_CID_AMP:
			g_warning("FBS_L2CAP_CID_AMP is NOT support");
			break;
		case FBS_L2CAP_CID_ATT:
			g_warning("NOT finished");
			break;
		case FBS_L2CAP_CID_LE_L2CAP:
			g_warning("NOT finished");
			break;
		case FBS_L2CAP_CID_SMP:
			g_warning("NOT finished");
			break;
		case FBS_L2CAP_CID_EDR_SM:
			g_warning("NOT finished");
			break;
		default:
			break;
	}
	int i;
	printf("L2cap :");
	for(i=0;i<len;i++){
		if(!(i%16)){printf("\n");}
		printf("%02x ", data[i]);
	} printf("\n");
}


void FBS_l2cap_connected(struct fbs_l2cap_link *link)
{
	g_assert(fbs_l2cap_tcb != NULL);
	struct fbs_l2cap_link_tcb *link_tcb;
	link_tcb = g_new0(struct fbs_l2cap_link_tcb, 1);
	link_tcb->conn = g_memdup(link, sizeof(struct fbs_l2cap_link));
	fbs_l2cap_tcb->link_list = g_slist_append(fbs_l2cap_tcb->link_list, link_tcb);
	g_debug("L2cap connected, type:%s, handle = 0x%04x",
			link_tcb->conn->type == FBS_L2CAP_LINK_BR_EDR?"BR/EDR":"BLE",
			link_tcb->conn->type == FBS_L2CAP_LINK_BR_EDR?
			   link_tcb->conn->link.edr.handle:link_tcb->conn->link.ble.handle);
}
static gint fbs_find_l2cap_link_by_handle_cb(
		gconstpointer list_data, gconstpointer user_data)
{
	struct fbs_l2cap_link_tcb *link_tcb = (struct fbs_l2cap_link_tcb*) list_data;
	guint16 *handle = (guint16*)user_data;
	if(link_tcb->conn->type == FBS_L2CAP_LINK_BR_EDR){
		return !(link_tcb->conn->link.edr.handle == *handle);
	}else if(link_tcb->conn->type == FBS_L2CAP_LINK_BLE){
		return !(link_tcb->conn->link.ble.handle == *handle);
	}else{
		return -1;
	}
}
void FBS_l2cap_disconnected(guint16 handle, guint8 reason)
{
	g_assert(fbs_l2cap_tcb != NULL);
	GSList *slist_node;
	slist_node = g_slist_find_custom(fbs_l2cap_tcb->link_list, &handle,
			fbs_find_l2cap_link_by_handle_cb);
	if(slist_node){
		struct fbs_l2cap_link_tcb *link_tcb = (struct fbs_l2cap_link_tcb*) slist_node->data;
		g_free(link_tcb->conn);
		g_free(link_tcb);
		fbs_l2cap_tcb->link_list =
			g_slist_delete_link (fbs_l2cap_tcb->link_list, slist_node);
		g_debug("L2cap disconnect, handle = 0x%04x", handle);
	}else{
		g_warning("L2cap link is missing when disconnected! (Handle = 0x%04x)", handle);
	}
	g_assert(fbs_l2cap_tcb != NULL);
}
void FBS_l2cap_send(guint16 handle, guint16 cid, gpointer *data, gint len)
{
	g_assert(fbs_l2cap_tcb != NULL);
}
