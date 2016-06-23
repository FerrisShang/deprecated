#include <stdio.h>
#include <unistd.h>
#include "att_io.h"
#include "bt_util.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

const struct att_io* att_io;

void conn_change_cb(bdaddr_t addr, int status, void *pdata)
{
	if(status == ATT_IO_STATUS_CONNECTED){
	}else{
	}
}
void receive(bdaddr_t addr, UINT8 *dat, UINT32 len, void *pdata)
{
	Log.v("message received length=%d", len);
}
void timeout_destroy_func(void *pdata)
{
	Log.v("timeout destroy ......");
}
int id;
bool timeout_func(void *pdata)
{
	Log.v("timeout......");
	return false;
}
int main(int argc, char *argv[])
{
	struct att_io_cb att_io_cb = {
		conn_change_cb,
		receive,
	};
	mgmt_setup(HCI_DEV_ID);
	le_set_random_address(HCI_DEV_ID);
	att_io = register_att_io(HCI_DEV_ID, &att_io_cb, NULL);
	le_set_advertise_parameters(HCI_DEV_ID);

	id = att_io->timeout_add(2000, timeout_func, (void*)att_io, timeout_destroy_func);
	att_io->timeout_remove(id);
	while(1){
		le_set_advertise_enable(HCI_DEV_ID);
		usleep(5000000);
		mem_dump();
	}
	bdaddr_t addr;
	int b[6];
	sscanf(argv[1], "%02X:%02X:%02X:%02X:%02X:%02X",
			&b[0],&b[1],&b[2],&b[3],&b[4],&b[5]);
	addr.b[0]=b[5];addr.b[1]=b[4];addr.b[2]=b[3];
	addr.b[3]=b[2];addr.b[4]=b[1];addr.b[5]=b[0];
	att_io->connect(&addr);
	usleep(10000);
	att_io->send(&addr, (UINT8*)"`1234567890-=", 13);
	while(1){
		usleep(5000000);
		Log.v("sleep");
	}
	return 0;
}
