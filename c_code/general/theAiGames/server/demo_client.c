#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void output(char *buf)
{
	fprintf(stdout,"%s", buf);
	fflush(stdout);
}
void* proc_read(void *p)
{
	char buf[1024];
	char out[1024];
	while(EOF != scanf("%s", buf)){
		sprintf(out, "client received : %s\n", buf);
		output(out);
	}
}
void* proc_write(void *p)
{
	char buf[1024];
	int cnt = 0;
	while(1){
		usleep(3000000);
		sprintf(buf, "client output %d\n", cnt++);
		output(buf);
	}
}
int main(int argc, char *argv[])
{
	pthread_t p_write, p_read;
	pthread_create(&p_write, NULL, proc_write, NULL);
	pthread_create(&p_read, NULL, proc_read, NULL);
	pthread_join(p_write, NULL);
	pthread_join(p_read, NULL);
}
