#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <glib.h>
#include "fbs_uart.h"
#include "fbs_btsnoop.h"

#define FBS_RESERVE_BUF_LEN 4 // [len(2) + rfu(2)] + hci
#define FBS_HCI_DATA_POINTER(p) (&(((guchar*)p)[FBS_RESERVE_BUF_LEN]))
#define FBS_HCI_DATA_LEN(p)     ((guchar)p[0]+ ((guchar)p[1]<<8))

static int uart_fd = -1;
static FILE *btsnoop_fd;
static GAsyncQueue *fbs_btsnoop_msg_queue;
static GMainLoop *fbs_uart_mainloop;
static gpointer fbs_uart_pdata;
static void (*FBS_uart_callback)(guchar *data, gint len, gpointer pdata);

static void fbs_btsnoop_msg_queue_destroy(gpointer data)
{
	guchar *p = data;
	g_slice_free1(FBS_HCI_MAX_LEN, p);
}
static void *fbs_start_btsnoop(void *p)
{
	gchar *path = p;
	btsnoop_fd = create_btsnoop_rec(path);
	if(btsnoop_fd == NULL){
		g_warning("Create btsnoop file failed(%s), debug disabled\n", path);
		return NULL;
	}
	fbs_btsnoop_msg_queue = g_async_queue_new_full(fbs_btsnoop_msg_queue_destroy);
	if(fbs_btsnoop_msg_queue == NULL){
		g_error("Not enough memory");
	}
	while(TRUE){
		struct msg_btsnoop *msg;
		msg = g_async_queue_pop(fbs_btsnoop_msg_queue);
		record_btsnoop(btsnoop_fd, msg);
		g_slice_free1(FBS_HCI_MAX_LEN, msg->hci_data);
		free_btsnoop_msg(msg);
		while((msg = g_async_queue_try_pop(fbs_btsnoop_msg_queue)) != NULL){
			record_btsnoop(btsnoop_fd, msg);
			g_slice_free1(FBS_HCI_MAX_LEN, msg->hci_data);
			free_btsnoop_msg(msg);
		}
		flush_btsnoop(btsnoop_fd);
	}
}

void FBS_enable_btsnoop(const gchar *path)
{
	pthread_t th;
	pthread_create(&th, NULL, fbs_start_btsnoop, (void*)path);
	usleep(10000);
}

static gboolean fbs_hci_recv_cb(gpointer data)
{
	guchar *p = data;
	FBS_uart_callback(FBS_HCI_DATA_POINTER(p), FBS_HCI_DATA_LEN(p), fbs_uart_pdata);
	return FALSE;
}

static void fbs_hci_recv_destroy_cb(gpointer user_data)
{
	guchar *p = user_data;
	g_slice_free1(FBS_HCI_MAX_LEN, p);
}

static gpointer fbs_read_uart_th(int fd)
{
	guchar *p;
	guchar buf[5];
	guint len;
	guint idx = 0;
	guint count = 0;
	count = read(fd, &buf[0], 1);
	if(count <= 0){
		g_error("Read uart data failed");
	}
	idx += 1;
	if(buf[0]== 2){ // ACL data
		len = 4;
		while ((count = read(fd, &buf[idx], len)) < len) {
			if(count <= 0){
				g_error("Read uart data failed");
			}
			idx += count;
			len -= count;
		}
		idx += count;
		len = buf[3] + (buf[4] << 8);
		p = g_slice_alloc(FBS_HCI_MAX_LEN);
		if(!p){
			g_error("Not enough memory");
		}
		memcpy(&p[FBS_RESERVE_BUF_LEN], buf, 5);
		while ((count = read(fd, &p[FBS_RESERVE_BUF_LEN+idx], len)) < len) {
			if(count <= 0){
				g_error("Read uart data failed");
			}
			idx += count;
			len -= count;
		}
		idx += count;
		p[0] = idx & 0xFF; //len
		p[1] = (idx >> 8) &0xFF;
		return p;
	}else if(buf[0] == 4){ // HCI event
		len = 2;
		while ((count = read(fd, &buf[idx], len)) < len) {
			if(count <= 0){
				g_error("Read uart data failed");
			}
			idx += count;
			len -= count;
		}
		idx += count;
		len = buf[2];
		p = g_slice_alloc(FBS_HCI_MAX_LEN);
		if(!p){
			g_error("Not enough memory");
		}
		memcpy(&p[FBS_RESERVE_BUF_LEN], buf, 3);
		while ((count = read(fd, &p[FBS_RESERVE_BUF_LEN+idx], len)) < len) {
			if(count <= 0){
				g_error("Read uart data failed");
			}
			idx += count;
			len -= count;
		}
		idx += count;
		p[0] = idx & 0xFF; //len
		p[1] = (idx >> 8) &0xFF;
		return p;
	}else{
		g_error("Unsupport hci data type");
		return NULL;
	}
}

void  FBS_uart_init(GMainLoop *mainloop,
		void (*recv_cb)(guchar *data, gint len, gpointer pdata),
		gpointer pdata)
{
	fbs_uart_mainloop = mainloop;
	FBS_uart_callback = recv_cb;
	fbs_uart_pdata = pdata;
}
void* FBS_uart_recv(void *unused)
{
	g_assert(fbs_uart_mainloop != NULL);
	g_assert(FBS_uart_callback != NULL);
#define FBS_ENV_DEV_PATH "FBS_ENV_DEV_PATH"
	const gchar *dev_path = g_getenv(FBS_ENV_DEV_PATH);
	if(!dev_path){
		g_error("Can not find device path");
	}
	uart_fd = open(dev_path, O_RDWR | O_NOCTTY);
	if(uart_fd <= 0){
		g_error("Can not open uart : %s", dev_path);
	}
	tcflush(uart_fd, TCIOFLUSH);
	while(g_main_loop_is_running(fbs_uart_mainloop) == FALSE){ g_usleep(1000); }
	while(TRUE){
		guchar *hci_data;
		if(g_main_loop_is_running(fbs_uart_mainloop) == FALSE){ break; }
		hci_data = fbs_read_uart_th(uart_fd);
		if(fbs_btsnoop_msg_queue != NULL){
			guint len = FBS_HCI_DATA_LEN(hci_data);
			guchar *uart_msg = g_slice_alloc(FBS_HCI_MAX_LEN);
			memcpy(uart_msg,  FBS_HCI_DATA_POINTER(hci_data), len);
			g_async_queue_push(fbs_btsnoop_msg_queue,
					new_btsnoop_msg((char*)uart_msg, len, BTDATA_DIR_RECV));
		}
		g_main_context_invoke_full(
				NULL,
				G_PRIORITY_DEFAULT,
				fbs_hci_recv_cb,
				hci_data,
				fbs_hci_recv_destroy_cb);
	}
	return NULL;
}

gint FBS_uart_send(guchar *data, gint len)
{
	g_assert(uart_fd > 0);
	if(fbs_btsnoop_msg_queue != NULL){
		guchar *uart_msg = g_slice_alloc(FBS_HCI_MAX_LEN);
		memcpy(uart_msg,  data, len);
		g_async_queue_push(fbs_btsnoop_msg_queue,
				new_btsnoop_msg((char*)uart_msg, len, BTDATA_DIR_SEND));
	}
	return write(uart_fd, data, len);
}
