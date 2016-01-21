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

#define LE_NAME "BlueZ Ancs"
int main(int argc, char *argv[])
{
	pid_t childpid;
	int status;
	while(1){
		childpid = fork();
		if ( childpid < 0 ){
			perror( "fork()" );
			exit( EXIT_FAILURE );
		}else if ( childpid == 0 ){
			ancs_start(parseData_cb, LE_NAME);
			exit(EXIT_SUCCESS);
		}else{
			waitpid( childpid, &status, 0 );
		}
		break;
	}
}
