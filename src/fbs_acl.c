#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <semaphore.h>
#include "fbs_uart.h"
#include "fbs_acl.h"
#include "fbs_l2cap.h"
#include "fbs_stack.h"

#define FBS_ACL_LOCAL_ACL_MTU 2048
#define FBS_ACL_LOCAL_ACL_PKT 32
#define FBS_ACL_LOCAL_SCO_MTU 255
#define FBS_ACL_LOCAL_SCO_PKT 32

#define FBS_ACL_PB_HANDLE(handle)        (handle & 0x0FFF)
#define FBS_ACL_PB_FLAG(handle)          ((handle >> 12) & 0x03)
#define FBS_ACL_PB_FLAG_CONTINUING_FRAG  0x01
#define FBS_ACL_PB_FLAG_FIRST_AUTO_FLUSH 0x02

typedef struct {
	guint8  type;
	guint16 handle;
	guint16 len;
	guint8  data[0];
} __attribute__ ((packed)) tFBS_acl_header;

struct fbs_acl_tcb {
	GSList *link_list; //struct fbs_acl_link_tcb
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

	guint8 *send_buf;
	guint16 send_buf_idx;
	guint16 send_handle;
	guint8 *recv_buf;
	guint16 recv_buf_idx;
	guint16 recv_len;
	guint16 recv_handle;
};

static struct fbs_acl_tcb *fbs_acl_tcb;

void FBS_acl_init(void)
{
	if(fbs_acl_tcb == NULL){
		fbs_acl_tcb = g_new0(struct fbs_acl_tcb, 1);
		fbs_acl_tcb->link_list = NULL;
		fbs_acl_tcb->local_acl_mtu     = FBS_ACL_LOCAL_ACL_MTU;
		fbs_acl_tcb->local_acl_max_pkt = FBS_ACL_LOCAL_ACL_PKT;
		fbs_acl_tcb->local_sco_mtu     = FBS_ACL_LOCAL_SCO_MTU;
		fbs_acl_tcb->local_sco_max_pkt = FBS_ACL_LOCAL_SCO_PKT;
		fbs_acl_tcb->recv_buf = g_malloc(FBS_ACL_LOCAL_ACL_MTU);
	}
}

void FBS_acl_destroy(void)
{
	if(fbs_acl_tcb == NULL){
		if(fbs_acl_tcb->sem_acl_count != NULL){
			sem_destroy(fbs_acl_tcb->sem_acl_count);
			g_free(fbs_acl_tcb->sem_acl_count);
		}
		if(fbs_acl_tcb->sem_sco_count != NULL){
			sem_destroy(fbs_acl_tcb->sem_sco_count);
			g_free(fbs_acl_tcb->sem_sco_count);
		}
		if(fbs_acl_tcb->recv_buf){
			g_free(fbs_acl_tcb->recv_buf);
		}
		g_free(fbs_acl_tcb);
	}
}

void FBS_acl_data_process(guchar *data, gint len)
{
	g_assert(fbs_acl_tcb != NULL);
	g_assert(fbs_acl_tcb->sem_acl_count != NULL);
	tFBS_acl_header *header = (tFBS_acl_header*)data;
	if(FBS_ACL_PB_FLAG(header->handle) == FBS_ACL_PB_FLAG_FIRST_AUTO_FLUSH){
		fbs_acl_tcb->recv_handle = FBS_ACL_PB_HANDLE(header->handle);
		fbs_acl_tcb->recv_len = header->data[0] + (header->data[1]<<8) +
			sizeof(guint16)+sizeof(guint16);//len+cid+data
		fbs_acl_tcb->recv_buf_idx = 0;
	}else if(FBS_ACL_PB_FLAG(header->handle) == FBS_ACL_PB_FLAG_CONTINUING_FRAG){
		if(fbs_acl_tcb->recv_handle != FBS_ACL_PB_HANDLE(header->handle)){
			g_error("ACL have bugs, Receive data handle mixed");
		}
	}

	if(fbs_acl_tcb->recv_buf_idx+header->len > FBS_ACL_LOCAL_ACL_MTU){
		g_error("ACL have bugs, data length exceed buffer size");
	}
	memcpy(&fbs_acl_tcb->recv_buf[fbs_acl_tcb->recv_buf_idx], header->data, header->len);
	fbs_acl_tcb->recv_buf_idx += header->len;
	if(fbs_acl_tcb->recv_buf_idx >= fbs_acl_tcb->recv_len){
		if(fbs_acl_tcb->recv_buf_idx > fbs_acl_tcb->recv_len){
			g_warning("ACL data length error");
		}
		FBS_l2cap_data_process(fbs_acl_tcb->recv_buf, fbs_acl_tcb->recv_len);
	}
}

void FBS_acl_comp_packets(guint16 handle, guint16 num)
{
	int i;
	g_assert(fbs_acl_tcb != NULL);
	g_assert(fbs_acl_tcb->sem_acl_count != NULL);

	for(i=0;i<num;i++){
		sem_post(fbs_acl_tcb->sem_acl_count);
	}
}

void FBS_get_acl_mtu(guint16 *acl_mtu, guint16 *acl_max_pkt)
{
	g_assert(fbs_acl_tcb != NULL);
	*acl_mtu = fbs_acl_tcb->local_acl_mtu;
	*acl_max_pkt = fbs_acl_tcb->local_acl_max_pkt;
}
void FBS_get_sco_mtu(guint8 *sco_mtu, guint16 *sco_max_pkt)
{
	g_assert(fbs_acl_tcb != NULL);
	*sco_mtu = fbs_acl_tcb->local_sco_mtu;
	*sco_max_pkt = fbs_acl_tcb->local_sco_max_pkt;
}
void FBS_record_chip_buffer_size(guint16 acl_mtu, guint16 acl_max_pkt,
		guint8 sco_mtu, guint16 sco_max_pkt)
{
	g_assert(fbs_acl_tcb != NULL);
	fbs_acl_tcb->chip_acl_mtu = acl_mtu;
	fbs_acl_tcb->chip_acl_max_pkt = acl_max_pkt;
	fbs_acl_tcb->chip_sco_mtu = sco_mtu;
	fbs_acl_tcb->chip_sco_max_pkt = sco_max_pkt;
	if(fbs_acl_tcb->sem_acl_count == NULL){
		fbs_acl_tcb->sem_acl_count = g_new0(sem_t, 1);
		sem_init(fbs_acl_tcb->sem_acl_count, 0, acl_max_pkt);
	}
	if(fbs_acl_tcb->sem_sco_count == NULL){
		fbs_acl_tcb->sem_sco_count = g_new0(sem_t, 1);
		sem_init(fbs_acl_tcb->sem_sco_count, 0, sco_max_pkt);
	}
}
