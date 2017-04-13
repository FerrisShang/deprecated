#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/un.h>
#include "data_bus.h"
#include "af_server.h"
#include "af_client.h"
#include "queue.h"

struct bus_server {
	data_bus_cb callback;
	struct queue *client_list;
	struct tcps* tcps;
	void *p; //user data
};
struct bus_client {
	struct tcpc* tcpc;
	data_bus_cb callback;
	int sock;
	int dev_type;
	int dev_id;
	void *p;
};

bool find_cli_by_sock_cb(const void *data, const void *match_data)
{
	struct bus_client *cli = (struct bus_client*)data;
	int *sock = (int*)match_data;
	assert(cli != NULL);
	assert(sock != NULL);
	return (cli->sock == *sock);
}
bool find_cli_by_id_cb(const void *data, const void *match_data)
{
	struct bus_client *cli = (struct bus_client*)data;
	struct bus_data *bdata = (struct bus_data*)match_data;
	assert(cli != NULL);
	assert(bdata != NULL);
	return (cli->dev_type == bdata->dev_type && cli->dev_id == bdata->dev_id);
}

static void srv_conn_cb(int sock, struct sockaddr_un *addr, void *pdata)
{
	struct bus_server *srv = (struct bus_server*) pdata;
	struct bus_client *cli;
	cli = (struct bus_client*)calloc(1, sizeof(struct bus_client));
	cli->sock = sock;
	cli->dev_type = DEV_TYPE_INVALID;
	queue_push_tail(srv->client_list, cli);
}

static void srv_recv_cb(int sock, struct sockaddr_un *addr,
		char *buf, int len, void *pdata)
{
	struct bus_server *srv = (struct bus_server*) pdata;
	struct bus_data *data = (struct bus_data*)buf;
	struct bus_client *cli;
	int pkg_num = len / sizeof(struct bus_data);
	int i;
	if((len % sizeof(struct bus_data)) != 0){
		printf("warning! server receive buffer maybe overrun, pkg_num=%d\n", pkg_num);
	}
	for(i=0;i<pkg_num;i++){
		cli= queue_find(srv->client_list, find_cli_by_sock_cb, &sock);
		if(!cli){
			printf("warning! socket missing on received\n");
			return;
		}else if(cli->dev_type == DEV_TYPE_INVALID){
			cli->dev_type = data->dev_type;
			cli->dev_id = data->dev_id;
		}
		srv->callback(data, srv->p);
		data++;
	}
}

static void srv_discon_cb(int sock, struct sockaddr_un *addr, void *pdata)
{
	struct bus_server *srv = (struct bus_server*) pdata;
	struct bus_client *cli;
	cli= queue_find(srv->client_list, find_cli_by_sock_cb, &sock);
	if(!cli){
		printf("warning! socket missing on disconnected\n");
		return;
	}
	queue_remove(srv->client_list, cli);
	free(cli);
}

struct bus_server* bus_server_create(data_bus_cb callback, void *p)
{
	struct bus_server *srv;
	srv = (struct bus_server*)malloc(sizeof(struct bus_server));
	if(!srv){
		printf("malloc bus_server error\n");
		exit(-1);
	}
	srv->client_list = queue_new();
	if(!srv->client_list){
		printf("malloc client list failed\n");
		exit(-1);
	}
	system("rm -f /tmp/exhibition.sock"); //delete socket file
	srv->tcps = tcps_create(srv_conn_cb, srv_recv_cb, srv_discon_cb, srv);
	usleep(50000);//waiting for tcp server startup
	if(!srv->tcps){
		printf("create tcps failed\n");
		exit(-1);
	}
	srv->callback = callback;
	srv->p = p;
	return srv;
}

int bus_server_send(struct bus_server* srv, struct bus_data *data)
{
	struct bus_client *cli;
	int res;
	cli= queue_find(srv->client_list, find_cli_by_id_cb, data);
	if(!cli){
		printf("warning! socket missing on send\n");
		return -1;
	}
	res = tcps_send(cli->sock, (char*)data, sizeof(struct bus_data));
	usleep(50000);//wait for sending done
	return res;
}

static void client_recv_cb(char *buf, int len, void *pdata)
{
	struct bus_client* cli = pdata;
	cli->callback((struct bus_data*)buf, cli->p);
}

static void client_close_cb(void *pdata)
{
	printf("warning! client disconnected\n");
}

struct bus_client* bus_client_create(int dev_type, int dev_id,
		data_bus_cb cb, void *p)
{
	struct bus_client* cli;
	cli = (struct bus_client*)calloc(1, sizeof(struct bus_client));
	if(!cli){
		printf("malloc client failed\n");
		exit(-1);
	}
	cli->tcpc = tcpc_create(client_recv_cb, client_close_cb, cli);
	usleep(50000);//waiting for tcp client startup
	if(!cli->tcpc){
		printf("client create failed\n");
		exit(-1);
	}
	cli->dev_type = dev_type;
	cli->dev_id = dev_id;
	cli->callback = cb;
	cli->p = p;
	return cli;
}

int bus_client_send(struct bus_client* cli, struct bus_data *data)
{
	int res;
	assert(cli != NULL);
	data->dev_type = cli->dev_type;
	data->dev_id = cli->dev_id;
	res = tcpc_send(cli->tcpc, (char*)data, sizeof(struct bus_data));
	usleep(50000);//waiting for client send
	return res;
}


