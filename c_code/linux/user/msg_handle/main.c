#include <stdio.h>
#include "msg_handle.h"
#include "mem_manage.h"

void handle_callback(int cmd, void *msg_data, void *user_data)
{
	printf("cmd = %d, msg = %s\n", cmd, (char*)msg_data);
}

int main(int argc, char *argv[])
{
	int i;
	struct msg_handle *msg_handle;
	char str[32];
	msg_handle = msg_handle_new(handle_callback, NULL);
	for(i=0;i<5;i++){
		sprintf(str, ">%04d<", i);
		msg_send(msg_handle, i, str);
	}
	msg_handle_destory(msg_handle);
	mem_dump();
	return 0;
}
