#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "data_bus.h"

struct bus_server* srv; 

int recv_cb(struct bus_data *data, void *p)
{
	char *str = (char*)p;
	switch(data->dev_type){
		case DEV_TYPE_LIGHT:{
			if(data->light.cmd == CMD_CONNECTED)
				printf("%s LIGHT - Connected\n", str);
			else if(data->light.cmd == CMD_DISCONNECTED)
				printf("%s LIGHT - Disconnected\n", str);
			else if(data->light.cmd == CMD_SEND_DATA){
				printf("%s LIGHT - status:%d\n", str, data->light.status);
				if(!strcmp(str, "Server recv")){ // Send data back to client
					data->light.status = ~data->light.status;
					bus_server_send(srv, data);
				}
			}
			break;
		}
		case DEV_TYPE_FAN:{
			break;
		}
		case DEV_TYPE_MIO_WATCH:{
			if(data->mio_watch.cmd == CMD_CONNECTED)
				printf("%s MIO_WATCH - Connected\n", str);
			else if(data->mio_watch.cmd == CMD_DISCONNECTED)
				printf("%s MIO_WATCH - Disconnected\n", str);
			else if(data->mio_watch.cmd == CMD_SEND_DATA){
				printf("%s MIO_WATCH - Heartrate:%d bpm\n",
						str, data->mio_watch.heart_rate);
			}
			break;
		}
		case DEV_TYPE_SZ_WATCH:{
			break;
		}
		case DEV_TYPE_SCALE:{
			break;
		}
		case DEV_TYPE_QRCODE:{
			break;
		}
		default:
			printf("device type not support\n");
			break;
	}
}
void* data_bus_client(void *p)
{
	struct bus_client *cli;
	struct bus_data data;
	int dev_type = (size_t)p;
	cli = bus_client_create(dev_type, 0, recv_cb, "Client recv");
	if(!cli){
		printf("Create bus client failed\n");
		return NULL;
	}

	if(dev_type == DEV_TYPE_LIGHT){
		data.light.cmd = CMD_CONNECTED;
		bus_client_send(cli, &data);
		while(1){
			sleep(1+(rand()&3));
			data.light.cmd = CMD_SEND_DATA;
			data.light.status = rand()&3;
			bus_client_send(cli, &data);
			usleep(100);
		}
	}else if(dev_type == DEV_TYPE_MIO_WATCH){
		while(1){
			int cnt = 10;
			sleep(3+(rand()&3));
			data.mio_watch.cmd = CMD_CONNECTED;
			bus_client_send(cli, &data);
			while(cnt--){
				sleep(1);
				data.mio_watch.cmd = CMD_SEND_DATA;
				data.mio_watch.heart_rate = 60+(rand()&31);
				bus_client_send(cli, &data);
			}
			sleep(1);
			data.mio_watch.cmd = CMD_DISCONNECTED;
			bus_client_send(cli, &data);
		}
	}else{
		while(1)sleep(~0);
	}
}

int main(int argc, char *argv[])
{
	int i;
	pthread_t th_s, th_c[DEV_TYPE_MAX];

	//Create data_bus server
	srv = bus_server_create(recv_cb, "Server recv");

	//Create data_bus clients
	for(i=0;i<DEV_TYPE_MAX;i++){
		pthread_create(&th_c[i], NULL, data_bus_client, (void*)(size_t)i);
	}
	while(1)sleep(~0);
}
