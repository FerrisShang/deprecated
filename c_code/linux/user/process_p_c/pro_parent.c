#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int find_pid(char *pro_name)
{
#define DATA_SIZE 80
	char str_buf[DATA_SIZE];
	FILE *pf = popen("ps","r");
	if(!pf){
		fprintf(stderr, "Could not open pipe for output.\n");
		return -1;
	}
	while(NULL != fgets(str_buf, DATA_SIZE , pf)){
		char *p;
		p = strstr(str_buf, pro_name);
		if(p != NULL){
			printf("%s", str_buf);
			pclose(pf);
			return atoi(str_buf);
		}
	}
	pclose(pf);
	return 0;
}
static void kill_all_pro_by_name(char *pro_name)
{
	pid_t ext_pro_pid;
	while((ext_pro_pid = find_pid(pro_name))>0){
		printf("kill extern progress, ext_pro_pid=%d\n", ext_pro_pid);
		kill(ext_pro_pid, SIGKILL);
	}
}
static int CreateMyProcess(char *pro_name)
{
	pid_t c_pid;
	c_pid = fork();
	if (c_pid < 0) {
		return -1;
	} else if (c_pid == 0){
		system(pro_name);
		exit(0);
	}
	return c_pid;
}
int main(int argc, char *argv[])
{
#define PRO_NAME "test_pro_child"
	pid_t c_pid;
	c_pid = CreateMyProcess("./"PRO_NAME);
	if (c_pid < 0) {
		printf("Create child progress failed \n");
		return 0;
	}
	printf("parent process sleep 3s\n");
	sleep(3);
	kill_all_pro_by_name(PRO_NAME);
	sleep(1);
	printf("parent progress exit\n");
}
