#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

/************************** pipes I/O begin ***************************/
#define NUM_PIPES          2
#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1
/* always in a pipe[], pipe[0] is for read and pipe[1] is for write */
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD(pipes)  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD(pipes) ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define CHILD_READ_FD(pipes)   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD(pipes)  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

struct stdio_pipe {
	int pipes[NUM_PIPES][2];
	char *app;
	char *argv;
};

struct stdio_pipe* stdio_open(char *app, char *argv[])
{
	int res;
	struct stdio_pipe *stdio_pipe;
	stdio_pipe = malloc(sizeof(struct stdio_pipe));
	if(!stdio_pipe){
		return NULL;
	}
	// pipes for parent to write and read
	res = pipe(stdio_pipe->pipes[PARENT_READ_PIPE]);
	res = pipe(stdio_pipe->pipes[PARENT_WRITE_PIPE]);

	if(!fork()) {
		dup2(CHILD_READ_FD(stdio_pipe->pipes), STDIN_FILENO);
		dup2(CHILD_WRITE_FD(stdio_pipe->pipes), STDOUT_FILENO);
		/* Close fds not required by child. Also, we don't
		   want the exec'ed program to know these existed */
		close(CHILD_READ_FD(stdio_pipe->pipes));
		close(CHILD_WRITE_FD(stdio_pipe->pipes));
		close(PARENT_READ_FD(stdio_pipe->pipes));
		close(PARENT_WRITE_FD(stdio_pipe->pipes));
		execv(app, argv);
	} else {
		/* close fds not required by parent */
		close(CHILD_READ_FD(stdio_pipe->pipes));
		close(CHILD_WRITE_FD(stdio_pipe->pipes));
		return stdio_pipe;
	}
}
int stdio_read(struct stdio_pipe* pipe, char *buf, int len)
{
	if(!pipe){
		return -1;
	}
	return read(PARENT_READ_FD(pipe->pipes), buf, len);
}
void stdio_write(struct stdio_pipe* pipe, char *data, int len)
{
	int res = write(PARENT_WRITE_FD(pipe->pipes), data, len);
}
/************************** pipes I/O end *****************************/
int main(int argc, char *argv[])
{
	int i, res, winner;
	char str[1024];
	struct stdio_pipe *app1, *app2;
	struct stdio_pipe *id1, *id2;
	int score[3] = {0};
	struct data data;
	FILE *fp;
	fp = fopen("./four.log", "a"); if(fp == NULL){ return -1; }
	if(argc != 3){
		return -1;
	}
	app1 = stdio_open(argv[1], NULL);
	app2 = stdio_open(argv[2], NULL);
	if(!app1 || !app2){
		printf("open error\n");
	}
	srand(time(NULL));
	while(1){//continue battle
		int record[64], record_cnt = 0;
		memset(&data, 0, sizeof(struct data));
		data.field_columns = 7;
		data.field_rows = 6;
		data.your_botid = 1;
		if(rand()&1){
			id1 = app1;
			id2 = app2;
		}else{
			id1 = app2;
			id2 = app1;
		}
		init_para(&data);

		sprintf(str, "settings your_botid 1\n");
		stdio_write(id1, str, strlen(str));
		sprintf(str, "settings your_botid 2\n");
		stdio_write(id2, str, strlen(str));
		sprintf(str, "settings field_columns 7\n");
		stdio_write(id1, str, strlen(str));
		stdio_write(id2, str, strlen(str));
		sprintf(str, "settings field_rows 6\n");
		stdio_write(id1, str, strlen(str));
		stdio_write(id2, str, strlen(str));
		//begin battle
		while(1){
			int len, pos;
			char tmp[80];
			//id 1
			get_field_str(str, &data);
			stdio_write(id1, str, strlen(str));
			sprintf(str, "action move 10000\n");
			stdio_write(id1, str, strlen(str));
			len = stdio_read(id1, str, 1024);
			sscanf(str, "%s %d", tmp, &pos);
			if(add_field(&data, 1, pos)<0){
				winner = (id1==app1?2:1);
				printf("app%d fail due to error place(%d),", id1 == app1?1:2, pos);
				break;
			}
			record[record_cnt++] = pos;
			//res = system("clear");dump_field(&data);usleep(100000);
			if(isFinish(&data, 1, pos)){
				winner = (id1==app1?1:2);
				printf("app%d win,", id1 == app1?1:2);
				break;
			}
			if(isFull(&data)){
				winner = 0;
				break;
			}
			//id 2
			get_field_str(str, &data);
			stdio_write(id2, str, strlen(str));
			sprintf(str, "action move 10000\n");
			stdio_write(id2, str, strlen(str));
			len = stdio_read(id2, str, 1024);
			sscanf(str, "%s %d", tmp, &pos);
			if(add_field(&data, 2, pos)<0){
				winner = (id2==app1?2:1);
				printf("app%d fail due to error place(%d),", id2 == app1?1:2, pos);
				break;
			}
			record[record_cnt++] = pos;
			//res = system("clear");dump_field(&data);usleep(100000);
			if(isFinish(&data, 2, pos)){
				winner = (id2==app1?1:2);
				printf("app%d win,", id2 == app1?1:2);
				break;
			}
			if(isFull(&data)){
				winner = 0;
				break;
			}
		}
		if(winner == 1){
			score[0]++;
		}else if(winner == 2){
			score[2]++;
		}else if(winner == 0){
			score[1]++;
		}
		printf("%d : %d : %d\n", score[0], score[1], score[2]);
		//output to log file
		sprintf(str, "%d ", record_cnt);
		for(i=0;i<record_cnt;i++){
			sprintf(str, "%s%d", str, record[i]);
		}
		sprintf(str, "%s\n", str);
		fputs(str, fp);
		fflush(fp);

		/*
		printf("app%d place first\n", id1 == app1?1:2);
		//dump_field(&data);
		for(i=0;i<record_cnt;i++){
			if((i%32)==0 && i!=0){
				printf("\n");
			}
			printf("%d ", record[i]);
			if(i&1){
				printf(" ");
			}
		}
		printf("\n");
		*/
	}
}
