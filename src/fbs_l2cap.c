#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <semaphore.h>
#include "fbs_l2cap.h"
#include "fbs_stack.h"
#include "fbs_uart.h"

#define FBS_L2CAP_LOCAL_ACL_MTU 2048
#define FBS_L2CAP_LOCAL_ACL_PKT 32
#define FBS_L2CAP_LOCAL_SCO_MTU 255
#define FBS_L2CAP_LOCAL_SCO_PKT 32

struct fbs_l2cap_link_tcb {
	struct fbs_l2cap_link *conn;
	guint8 *send_buf;
	guint8 *recv_buf;
	gboolean isSendPending;
	gboolean isRecvPending;
};

struct fbs_l2cap_tcb {
	GSList *link_list; //struct fbs_l2cap_link_tcb
	guint16 local_acl_mtu;
	guint16 local_acl_max_pkt;
	guint8  local_sco_mtu;
	guint16 local_sco_max_pkt;
	guint16 chip_acl_mtu;
	guint16 chip_acl_max_pkt;
	guint8  chip_sco_mtu;
	guint16 chip_sco_max_pkt;

	sem_t *sem_acl_count;
	sem_t *sem_sco_count;
};

static struct fbs_l2cap_tcb *fbs_l2cap_tcb;

void FBS_l2cap_init(void)
{
	if(fbs_l2cap_tcb == NULL){
		fbs_l2cap_tcb = g_new0(struct fbs_l2cap_tcb, 1);
		fbs_l2cap_tcb->link_list = NULL;
		fbs_l2cap_tcb->local_acl_mtu     = FBS_L2CAP_LOCAL_ACL_MTU;
		fbs_l2cap_tcb->local_acl_max_pkt = FBS_L2CAP_LOCAL_ACL_PKT;
		fbs_l2cap_tcb->local_sco_mtu     = FBS_L2CAP_LOCAL_SCO_MTU;
		fbs_l2cap_tcb->local_sco_max_pkt = FBS_L2CAP_LOCAL_SCO_PKT;
	}
}
static void fbs_destroy_list(gpointer data)
{
	struct fbs_l2cap_link_tcb *link_tcb = (struct fbs_l2cap_link_tcb*) data;
	g_free(link_tcb->conn);
	g_free(link_tcb->send_buf);
	g_free(link_tcb->recv_buf);
	g_free(link_tcb);
}
void FBS_l2cap_destroy(void)
{
	if(fbs_l2cap_tcb != NULL){
		if(fbs_l2cap_tcb->link_list != NULL){
			g_slist_free_full(fbs_l2cap_tcb->link_list, fbs_destroy_list);
		}
		if(fbs_l2cap_tcb->sem_acl_count != NULL){
			sem_destroy(fbs_l2cap_tcb->sem_acl_count);
			g_free(fbs_l2cap_tcb->sem_acl_count);
		}
		if(fbs_l2cap_tcb->sem_sco_count != NULL){
			sem_destroy(fbs_l2cap_tcb->sem_sco_count);
			g_free(fbs_l2cap_tcb->sem_sco_count);
		}
		g_free(fbs_l2cap_tcb);
	}
}

void FBS_l2cap_data_process(guchar *data, gint len)
{
	int i;
	printf("L2cap :");
	for(i=0;i<len;i++){
		if(!(i%16)){printf("\n");}
		printf("%02x ", data[i]);
	} printf("\n");
}

void FBS_l2cap_comp_packets(guint16 handle, guint16 num)
{
	int i;
	g_assert(fbs_l2cap_tcb != NULL);
	g_assert(fbs_l2cap_tcb->sem_acl_count != NULL);

	for(i=0;i<num;i++){
		sem_post(fbs_l2cap_tcb->sem_acl_count);
	}
}

void FBS_get_acl_mtu(guint16 *acl_mtu, guint16 *acl_max_pkt)
{
	g_assert(fbs_l2cap_tcb != NULL);
	*acl_mtu = fbs_l2cap_tcb->local_acl_mtu;
	*acl_max_pkt = fbs_l2cap_tcb->local_acl_max_pkt;
}
void FBS_get_sco_mtu(guint8 *sco_mtu, guint16 *sco_max_pkt)
{
	g_assert(fbs_l2cap_tcb != NULL);
	*sco_mtu = fbs_l2cap_tcb->local_sco_mtu;
	*sco_max_pkt = fbs_l2cap_tcb->local_sco_max_pkt;
}
void FBS_record_chip_buffer_size(guint16 acl_mtu, guint16 acl_max_pkt,
		guint8 sco_mtu, guint16 sco_max_pkt)
{
	g_assert(fbs_l2cap_tcb != NULL);
	fbs_l2cap_tcb->chip_acl_mtu = acl_mtu;
	fbs_l2cap_tcb->chip_acl_max_pkt = acl_max_pkt;
	fbs_l2cap_tcb->chip_sco_mtu = sco_mtu;
	fbs_l2cap_tcb->chip_sco_max_pkt = sco_max_pkt;
	if(fbs_l2cap_tcb->sem_acl_count == NULL){
		fbs_l2cap_tcb->sem_acl_count = g_new0(sem_t, 1);
		sem_init(fbs_l2cap_tcb->sem_acl_count, 0, acl_max_pkt);
	}
	if(fbs_l2cap_tcb->sem_sco_count == NULL){
		fbs_l2cap_tcb->sem_sco_count = g_new0(sem_t, 1);
		sem_init(fbs_l2cap_tcb->sem_sco_count, 0, sco_max_pkt);
	}
}

void FBS_l2cap_connected(struct fbs_l2cap_link *link)
{
	g_assert(fbs_l2cap_tcb != NULL);
	g_assert(fbs_l2cap_tcb->sem_acl_count != NULL);
	struct fbs_l2cap_link_tcb *link_tcb;
	link_tcb = g_new0(struct fbs_l2cap_link_tcb, 1);
	link_tcb->conn = g_memdup(link, sizeof(struct fbs_l2cap_link));
	link_tcb->send_buf = g_malloc(fbs_l2cap_tcb->chip_acl_max_pkt);
	link_tcb->recv_buf = g_malloc(FBS_L2CAP_LOCAL_ACL_MTU);
	link_tcb->isSendPending = FALSE;
	link_tcb->isRecvPending = FALSE;
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
		g_free(link_tcb->send_buf);
		g_free(link_tcb->recv_buf);
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
	g_assert(fbs_l2cap_tcb->sem_acl_count != NULL);
	sem_wait(&fbs_l2cap_tcb->sem_acl_count);
	guchar *hci_data = g_slice_alloc(FBS_RESERVE_BUF+4+len);
	hci_data[0] = ((4 + len) >> 0) & 0xFF;
	hci_data[1] = ((4 + len) >> 8) & 0xFF;
	hci_data[FBS_RESERVE_BUF+0] = 1; //cmd
	hci_data[FBS_RESERVE_BUF+1] = (opcode >> 0) & 0xFF;
	hci_data[FBS_RESERVE_BUF+2] = (opcode >> 8) & 0xFF;
	hci_data[FBS_RESERVE_BUF+3] = (guchar)len;
	memcpy(&hci_data[FBS_RESERVE_BUF+4], data, len);
	fbs_add_hci_tout(1, opcode);
	g_main_context_invoke_full(
			NULL,
			G_PRIORITY_DEFAULT,
			fbs_uart_send_cb,
			hci_data,
			fbs_uart_send_destroy_cb);
}
