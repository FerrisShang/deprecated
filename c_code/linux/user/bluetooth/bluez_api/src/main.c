#include <stdio.h>
#include <unistd.h>
#include "att_io.h"
#include "bt_util.h"
#include "mgmt.h"
#include "log.h"
#define HCI_DEV_ID 0

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
int main(int argc, char *argv[])
{
	struct att_io_cb att_io_cb = {
		conn_change_cb,
		receive,
	};
	mgmt_setup(HCI_DEV_ID);
	le_set_random_address(HCI_DEV_ID);
	register_att_io(HCI_DEV_ID, &att_io_cb, NULL);
	le_set_advertise_parameters(HCI_DEV_ID);
	while(1){
		usleep(2000000);
		le_set_advertise_enable(HCI_DEV_ID);
	}
	return 0;
}
