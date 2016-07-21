#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "wi_bus.h"
#include "log.h"
/*
	Log.v("request send data:");
	Log.v("\tfrom:self to 0x%s",
			hex2str(msg->id, msg->len));
	Log.v("\tlength:%d", 16);
	Log.v("\tdata:%s", hex2str(buf, 32));
	Log.v("\t     %s", hex2str(buf+32, 32));
*/

void recv_cb(wiaddr_t *remote_id, char *buf, int len, void *user_data)
{
	Log.v("length:%d", len);
	Log.v("data:%s", hex2str(buf, 32));
	Log.v("     %s", hex2str(buf+32, 32));
}
void disc_cb(void *user_data)
{
	Log.v("exit");
	exit(0);
}
int main(int argc, char *argv[])
{
#if 0
	wi_bus_server_run();
#else
	wiaddr_t addr;
	memcpy(&addr, "12345678", 8);
	wi_register(&addr, recv_cb, disc_cb, NULL);
	wi_send(&addr, "123123123", 10, 0);
	while(1){
		usleep(10000000);
	}
#endif
	return 0;
}

