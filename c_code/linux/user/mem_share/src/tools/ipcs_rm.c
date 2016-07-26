#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA_SIZE 1024
#define IPCS_ALL  "ipcs -a "
#define IPCRM_M   "ipcrm -m "
#define IPCRM_S   "ipcrm -s "

#define SHM_FLAG  "Shared Memory Segments"
#define SEM_FLAG  "Semaphore Arrays"
#define ID_FLAG  "fshang"

enum{
	UNKNOWN_TO_KILL,
	SHM_TO_KILL,
	SEM_TO_KILL,
};
static void process_cmd(char *cmd,
		void (*process_cmd_cb)(FILE *pf, void *pdata), void *pdata)
{
	FILE *pf = popen(cmd,"r");
	if(!pf){
		fprintf(stderr, "Could not open pipe for output.\n");
		return;
	}
	process_cmd_cb(pf, pdata);
	pclose(pf);
	return;
}

static void ipcs_rm_cb(FILE *pf, void *pdata)
{
	char *id_flag = pdata;
	char str_buf[DATA_SIZE];
	char flag = UNKNOWN_TO_KILL;
	while(NULL != fgets(str_buf, DATA_SIZE , pf)){
		char *p = str_buf;
		if(strstr(str_buf, SHM_FLAG)){
			flag = SHM_TO_KILL;
		}else if(strstr(str_buf, SEM_FLAG)){
			flag = SEM_TO_KILL;
		}else if(strstr(str_buf, id_flag)){
			char key[32], semid[32], perms[32], nsems[32], owner[32];
			sscanf(str_buf, "%s %s %s %s %s", 
					key, semid, perms, nsems, owner);
			if(flag == SHM_TO_KILL){
				sprintf(str_buf, "%s %s", IPCRM_M, semid);
				system(str_buf);
			}else if(flag == SEM_TO_KILL){
				sprintf(str_buf, "%s %s", IPCRM_S, semid);
				system(str_buf);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc == 1){
		process_cmd(IPCS_ALL, ipcs_rm_cb, ID_FLAG);
	}else{
		process_cmd(IPCS_ALL, ipcs_rm_cb, argv[1]);
	}
	return 0;
}
