#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdio_open.h"

void* proc_read(void *p)
{
	struct stdio_pipe* pipe = p;
	char buf[1024], out[1024];
	int num;
	while(1){
		num = stdio_read(pipe, buf, 1024);
		if(num > 0){
			snprintf(out, num, "%s", buf);
			printf(">> {%s}(%d)\n", out, num);
		}else{
			printf("IO error or process exit\n");
			exit(0);
		}
	}
}
void* proc_write(void *p)
{
	struct stdio_pipe* pipe = p;
	char buf[1024];
	int cnt = 1;
	while(1){
		usleep(2000000);
		sprintf(buf, "%d\n", cnt++);
		printf("<< server output : %s", buf);
		stdio_write(pipe, buf, strlen(buf));
	}
}
void main(int argc, char *argv[])
{
	struct stdio_pipe *pipe;
	pthread_t p_write, p_read;
	pipe = stdio_open(argv[1], &argv[1]);
	pthread_create(&p_write, NULL, proc_write, pipe);
	pthread_create(&p_read, NULL, proc_read, pipe);
	pthread_join(p_write, NULL);
	pthread_join(p_read, NULL);
}
