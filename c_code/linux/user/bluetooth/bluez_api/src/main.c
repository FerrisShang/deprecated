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
	mgmt_setup(HCI_DEV_ID);
	le_set_random_address(HCI_DEV_ID);
	le_set_advertise_parameters(HCI_DEV_ID);

	while(1){
		le_set_advertise_enable(HCI_DEV_ID);
		usleep(5000000);
		mem_dump();
	}
	return 0;
}
