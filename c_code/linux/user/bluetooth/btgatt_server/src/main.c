#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>

#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"

#include "src/mainloop.h"
#include "src/util.h"
#include "src/att.h"
#include "src/queue.h"
#include "src/gatt-db.h"
#include "src/gatt-client.h"

#include "AncsParser.h"

void parseData_cb(resp_data_t *getNotifCmd, void *user_data)
{
	if(getNotifCmd->state != STATE_DATA_ADDED_NEW){
		return;
	}
	printf("==========================================\n");
	printf("st:%d\n", getNotifCmd->state);
	printf("uuid:0x%08x\n", getNotifCmd->notif_uid);
	printf("app:%s\n", getNotifCmd->appId);
	printf("tit:%s\n", getNotifCmd->title);
	printf("sit:%s\n", getNotifCmd->subtitle);
	printf("msg:%s\n", getNotifCmd->message);
	printf("msl:%d\n", getNotifCmd->msg_len);
	printf("date:%s\n", getNotifCmd->date);
	printf("\n\n");
}

int main(int argc, char *argv[])
{
	while(1){
		ancs_start(parseData_cb);
		mem_dump();
		break;
	}
	return 0;
}
