#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "src/ancs.h"

static void read_local_name(int hdev, char *name);

void parseData_cb(resp_data_t *getNotifCmd, void *user_data)
{
	printf("==========================================\n");
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
	pid_t childpid;
	char ancs_name[248];
	int status;
	read_local_name(0, ancs_name);
	strcat(ancs_name, "-nc");
	while(1){
		childpid = fork();
		if ( childpid < 0 ){
			perror( "fork()" );
			exit( EXIT_FAILURE );
		}else if ( childpid == 0 ){
			ancs_start(parseData_cb, ancs_name);
			exit(EXIT_SUCCESS);
		}else{
			waitpid( childpid, &status, 0 );
		}
	}
}

static void read_local_name(int hdev, char *name)
{
	read_local_name_rp rp;
	struct hci_request rq;
	int dd, err, ret;

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		err = -errno;
		fprintf(stderr, "Could not open device: %s(%d)\n",
							strerror(-err), -err);
		exit(1);
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_HOST_CTL;
	rq.ocf = OCF_READ_LOCAL_NAME;
	rq.rparam = &rp;
	rq.rlen = READ_LOCAL_NAME_RP_SIZE;

	ret = hci_send_req(dd, &rq, 1000);
	if (ret < 0) {
		err = -errno;
		fprintf(stderr, "Can't read local name(%d): "
				"%s (%d)\n", hdev, strerror(-err), -err);
		exit(1);
	}
	strcpy(name, (char*)rp.name);

	hci_close_dev(dd);
}
