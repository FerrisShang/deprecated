#include <stdio.h>
#include <pthread.h>
#include "att_io.h"
#include "bt_util.h"
#include "log.h"
#include "io.h"

static int att_io_connect(bdaddr_t addr);
static int att_io_send(bdaddr_t addr, UINT8 *dat, UINT32 len);
static void *listen_thd(void *argv);
static void *receive_thd(void *argv);

static void *user_data;
static int hci_device;
static struct att_io_cb att_io_cb;
static struct att_io att_io = {
	att_io_connect,
	att_io_send
};
#define IO_DATA_NUM 32
struct io_data {
	struct io *io;
	int fd;
	bdaddr_t addr;
	void *pdata;
}io_data[IO_DATA_NUM ];

const struct att_io* register_att_io(int hdev, struct att_io_cb *io_cb, void *pdata)
{
	int ret;
	pthread_t pthread_listen;
	pthread_t pthread_receive;
	if(io_cb == NULL || 
			io_cb->conn_change_cb == NULL || 
			io_cb->receive == NULL){
		Log.e("regist callbacks failed");
		return NULL;
	}
	if(att_io_cb.conn_change_cb != NULL && att_io_cb.receive != NULL){
		Log.e("already registed");
		return NULL;
	}
	att_io_cb = *io_cb;
	user_data = pdata;
	hci_device = hdev;

	ret = pthread_create(&pthread_receive, NULL, receive_thd, user_data);
	if(ret){  
		Log.e("create thread \"%s\" failed", "receive_thd");
		return NULL;  
	}  
	ret = pthread_create(&pthread_listen, NULL, listen_thd, user_data);
	if(ret){  
		Log.e("create thread \"%s\" failed", "listen_thd");
		return NULL;  
	}  
	return &att_io;
}

static int att_io_connect(bdaddr_t addr)
{
	return ATT_IO_SUCCESS;
}
static int att_io_send(bdaddr_t addr, UINT8 *dat, UINT32 len)
{
	return ATT_IO_SUCCESS;
}

static struct io_data* search_empty_io_data(void)
{
	int i;
	for(i=0;i<IO_DATA_NUM;i++){
		if(io_data[i].io == NULL){
			return &io_data[i];
		}
	}
	return NULL;
}
static struct io_data* search_io_data(struct io *io)
{
	int i;
	for(i=0;i<IO_DATA_NUM;i++){
		if(io_data[i].io == io){
			return &io_data[i];
		}
	}
	return NULL;
}
static struct io_data* new_io_data(int fd, bdaddr_t addr)
{
	struct io_data* io_data = search_empty_io_data();
	if(!io_data){
		return NULL;
	}
	io_data->io= io_new(fd);
	if(!io_data->io){
		return NULL;
	}
	io_data->fd = fd;
	io_data->addr = addr;
	return io_data;
}
static void free_io_data(struct io *io)
{
	struct io_data* io_data = search_io_data(io);
	if(io_data){
		io_destroy(io_data->io);
		io_data->io = NULL;
	}
}
static bool io_read_callback(struct io *io, void *pdata)
{
	struct io_data *io_data = pdata;
	int read_num;
	char buf[1024];
	read_num = read(io_data->fd, buf, 1024);
	if(read_num < 0)return false;
	//dump_btaddr("ATT: Read from:", &io_data->addr);
	att_io_cb.receive(io_data->addr, (unsigned char*)buf, read_num, user_data);
	return true;
}
static bool io_destroy_callback(struct io *io, void *pdata)
{
	struct io_data *io_data = pdata;
	dump_btaddr("ATT: Disconnected from:", &io_data->addr);
	att_io_cb.conn_change_cb(io_data->addr, ATT_IO_STATUS_DISCONNECTED, user_data);
	free_io_data(io);
	return true;
}
static void connected_cb(int fd, bdaddr_t addr)
{
	struct io_data *io_data = new_io_data(fd, addr);
	if(!io_data){
		Log.e("memory error! %s@%d", __func__, __LINE__);
		return;
	}
	io_set_read_handler(io_data->io, io_read_callback, io_data, NULL);
	io_set_disconnect_handler(io_data->io, io_destroy_callback, io_data, NULL);
	dump_btaddr("ATT: Connected from:", &io_data->addr);
	att_io_cb.conn_change_cb(io_data->addr, ATT_IO_STATUS_CONNECTED, user_data);
	return;
}
static void *listen_thd(void *argv)  
{
	int security = BT_SECURITY_HIGH;
	l2cap_le_att_listen_and_accept(hci_device, security, connected_cb);
	return NULL;
}
static void *receive_thd(void *argv)  
{
	mainloop_init();
	mainloop_run();
	return NULL;
}
