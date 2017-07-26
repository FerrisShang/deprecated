#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <glib.h>
#include "fbs_stack.h"

#define HCI_SEND_DATA_LEN(d) (d[0]==1?(4+d[3]):(5+d[3]+(d[4]<<8)))
#define HCI_RECV_DATA_LEN(d) (d[0]==4?(3+d[2]):(5+d[3]+(d[4]<<8)))

static int uart_fd = -1;

static gboolean fbs_hci_recv_cb(gpointer user_data)
{
	FBS_hci_report((guchar*) user_data);
	return FALSE;
}

static void fbs_hci_recv_destroy_cb(gpointer user_data)
{
	guchar *p = user_data;
	g_slice_free1(HCI_RECV_DATA_LEN(p), user_data);
}

static gpointer fbs_read_uart(int fd)
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
		g_warning("Read ACL data may contain bugs, need check");
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
		p = g_slice_alloc(len + 5);
		if(!p){
			g_error("Not enough memory");
		}
		memcpy(p, buf, 5);
		while ((count = read(fd, &p[idx], len)) < len) {
			if(count <= 0){
				g_error("Read uart data failed");
			}
			idx += count;
			len -= count;
		}
		idx += count;
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
		p = g_slice_alloc(len + 3);
		if(!p){
			g_error("Not enough memory");
		}
		memcpy(p, buf, 3);
		while ((count = read(fd, &p[idx], len)) < len) {
			if(count <= 0){
				g_error("Read uart data failed");
			}
			idx += count;
			len -= count;
		}
		idx += count;
		return p;
	}else{
		g_error("Unsupport hci data type");
		return NULL;
	}
}

void* fbs_hci_recv(void *p)
{
#define FBS_ENV_DEV_PATH "FBS_ENV_DEV_PATH"
	GMainLoop *mainloop = p;
	while(g_main_loop_is_running(mainloop) == FALSE){ g_usleep(1000); }
	const gchar *dev_path = g_getenv(FBS_ENV_DEV_PATH);
	if(!dev_path){
		g_error("Can not find device path");
	}
	uart_fd = open(dev_path, O_RDWR | O_NOCTTY);
	if(uart_fd <= 0){
		g_error("Can not open uart : %s", dev_path);
	}
	while(TRUE){
		guchar *hci_data;
		if(g_main_loop_is_running(mainloop) == FALSE){ break; }
		hci_data = fbs_read_uart(uart_fd);
		g_main_context_invoke_full(
				NULL,
				G_PRIORITY_DEFAULT,
				fbs_hci_recv_cb,
				hci_data,
				fbs_hci_recv_destroy_cb);
	}
	return NULL;
}

static gboolean fbs_hci_send_cb(gpointer user_data)
{
	FBS_hci_send_raw((guchar*) user_data);
	return FALSE;
}

static void fbs_hci_send_destroy_cb(gpointer user_data)
{
	guchar *p = user_data;
	g_slice_free1(HCI_SEND_DATA_LEN(p), user_data);
}

guint FBS_hci_send(guchar *data, guint len)
{
	g_assert(uart_fd > 0);
	return write(uart_fd, data, len);
}

void* fbs_command_recv(void *p) //RFU
{
	GMainLoop *mainloop = p;
	while(g_main_loop_is_running(mainloop) == FALSE || uart_fd < 0){ g_usleep(1000); }
	while(TRUE){
		if(g_main_loop_is_running(mainloop) == FALSE){ break; }
		guchar hci_read_name[] = { 0x01, 0x14, 0x0c, 0x00 };
		guchar *hci_data = g_slice_alloc(sizeof(hci_read_name));
		memcpy(hci_data, hci_read_name, sizeof(hci_read_name));
		g_main_context_invoke_full(
				NULL,
				G_PRIORITY_DEFAULT,
				fbs_hci_send_cb,
				hci_data,
				fbs_hci_send_destroy_cb);
		g_usleep(~0);
	}
	return NULL;
}
