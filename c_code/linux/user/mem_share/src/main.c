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
	wi_unregister(NULL);
	mem_dump();
	exit(0);
}
int main(int argc, char *argv[])
{
#if 0
	wi_bus_server_run();
#else
	wiaddr_t laddr, raddr;
	if(argc == 1){
		return 0;
	}
	memcpy(&laddr, argv[1], 8);
	wi_register(&laddr, recv_cb, disc_cb, NULL);
	while(1){
		char l_addr[12], r_addr[12], buf[80];
		if(scanf("%s %s %s", l_addr, r_addr, buf) == EOF){
			return 0;
		}
		memcpy(&laddr, l_addr, sizeof(wiaddr_t));
		memcpy(&raddr, r_addr, sizeof(wiaddr_t));
		if(!strcmp(buf, "exit")){
			break;
		}
		Log.v("begin send");
		if(!strcmp(buf, "test")){
			int i;
			for(i=0;i<10;i++){
				sprintf(buf, "%d", i);
				if(wi_send(&laddr, &raddr, buf, 1000, (i%10) == 0?
							WI_FLAG_PRIORITY_HIGH : WI_FLAG_PRIORITY_LOW)<0){
					break;
				}
			}
			break;
		}else{
			wi_send(&laddr, &raddr, buf, strlen(buf), 0);
		}
		Log.v("send done");
	}
	wi_unregister(NULL);
	mem_dump();
#endif
	return 0;
}

