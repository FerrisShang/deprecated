#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "wi_bus.h"
#include "mem_manage.h"
#include "log.h"
void recv_cb(wiaddr_t *remote_id, char *buf, int len, void *user_data)
{
	Log.v("From ID:%s", hex2str(remote_id, 8));
	usleep(500000);
	Log.v("length:%d", len);
	Log.v("data:%s", hex2str(buf, 32));
	Log.v("     %s", hex2str(buf+32, 32));
}
void disc_cb(void *user_data)
{
	Log.d("wi_bus disconnected");
	usleep(500000);
	wi_unregister();
	mem_dump();
	exit(0);
}
int main(int argc, char *argv[])
{
#if 0
	wi_bus_server_run();
#else
	wiaddr_t addr;
	if(argc == 1){
		return 0;
	}
	memcpy(&addr, argv[1], 8);
	wi_register(&addr, recv_cb, disc_cb, NULL);
	while(1){
		char r_addr[12], buf[80];
		if(scanf("%s %s", r_addr, buf) == EOF){
			return 0;
		}
		memcpy(&addr, r_addr, sizeof(wiaddr_t));
		if(!strcmp(buf, "exit")){
			break;
		}
		Log.v("begin send");
		if(!strcmp(buf, "test")){
			int i;
			for(i=0;i<100000;i++){
				sprintf(buf, "%d", i);
				if(wi_send(&addr, buf, 1000, (i%10) == 0?
							WI_FLAG_PRIORITY_HIGH : WI_FLAG_PRIORITY_LOW)<0){
					break;
				}
			}
		}else{
			wi_send(&addr, buf, strlen(buf), 0);
		}
		Log.v("send done");
	}
	wi_unregister();
	mem_dump();
#endif
	return 0;
}

