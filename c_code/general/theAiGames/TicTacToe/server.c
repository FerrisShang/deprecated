#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "ttt.h"

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
	if(res<0){
		printf("init pipes error\n");
	}

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
	return NULL;
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
	if(res < 0){
		printf("write pipes erroror\n");
	}
}
/************************** pipes I/O end *****************************/

void get_field_str(char *str, struct ttt *ttt)
{
	char field[9][9];
	char buf[] = "update game field ";
	int i, j;
	memset(str, 0, 1024);
	ttt_get_field(ttt, field);
	strcpy(str, buf);
	str += sizeof(buf)-1;
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			str[2*(i*9+j)] = '0'+field[i][j];
			str[2*(i*9+j)+1] = ',';
		}
	}
	strcat(str, "\n");
}
void get_m_field_str(char *str, int *m_field)
{
	char buf[] = "update game macroboard ";
	int i;
	memset(str, 0, 1024);
	strcpy(str, buf);
	for(i=0;i<8;i++){
		sprintf(str,"%s%d,", str, m_field[i]);
	}
	sprintf(str,"%s%d", str, m_field[i]);
	strcat(str, "\n");
}
void set_m_field(struct ttt *ttt, int *m_field, point_t p)
{
	if(GB2(ttt->m_field, PO2OF(ttt,p)) == NORMAL){
		memset(m_field, 0, 9*sizeof(int));
		m_field[PO2OF(ttt,p)] = -1;
	}else{
		int i;
		for(i=0;i<TTT_BLK_NUM;i++){
			if(GB2(ttt->m_field, i) == NORMAL){
				m_field[i] = -1;
			}else{
				m_field[i] = 0;
			}
		}
	}
	int i;for(i=0;i<9;i++)printf("%d",m_field[i]);
	printf(" x:%d y:%d\n\n", POINT_X(p), POINT_Y(p));
}
int main(int argc, char *argv[])
{
	int i, r, winner;
	char str[1024];
	int m_field[9];
	struct stdio_pipe *app1, *app2;
	struct stdio_pipe *id1, *id2;
	int score[3] = {0};
	struct ttt *ttt = ttt_create();
	FILE *fp;

	fp = fopen("./ttt.log", "a"); if(fp == NULL){ return -1; }
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
		if(rand()&1){
			id1 = app1;
			id2 = app2;
		}else{
			id1 = app2;
			id2 = app1;
		}

		memset(ttt->s_field, 0, sizeof(ttt->s_field));
		memset(&ttt->m_field, 0, sizeof(int));
		for(i=0;i<9;i++){m_field[i]=-1;}
		sprintf(str, "settings your_botid 1\n");
		stdio_write(id1, str, strlen(str));
		sprintf(str, "settings your_botid 2\n");
		stdio_write(id2, str, strlen(str));
		//begin battle
		while(1){
			int x,y;
			char tmp[80];
			//id 1
			printf("%s", ttt_dump_field(ttt));
			get_field_str(str, ttt);
			stdio_write(id1, str, strlen(str));
			get_m_field_str(str, m_field);
			stdio_write(id1, str, strlen(str));
			sprintf(str, "action move 10000\n");
			stdio_write(id1, str, strlen(str));
			stdio_read(id1, str, 1024);
			sscanf(str, "%s %d %d", tmp, &x, &y);
			if(ttt_add(ttt, ID_P1, POINT(x,y))<0){
				winner = (id1==app1?2:1);
				printf("app%d fail due to error place(%d,%d),", id1 == app1?1:2, x,y);
				break;
			}
			record[record_cnt++] = POINT(x,y);
			r = ttt_isMFinish(ttt);
			if(r == ID_P1){
				winner = (id1==app1?1:2);
				printf("app%d win,", id1 == app1?1:2);
				break;
			}else if(r == DRAW){
				winner = 0;
				break;
			}
			set_m_field(ttt, m_field, POINT(x,y));
			//id 2
			printf("%s", ttt_dump_field(ttt));
			get_field_str(str, ttt);
			stdio_write(id2, str, strlen(str));
			get_m_field_str(str, m_field);
			stdio_write(id2, str, strlen(str));
			sprintf(str, "action move 10000\n");
			stdio_write(id2, str, strlen(str));
			stdio_read(id2, str, 1024);
			sscanf(str, "%s %d %d", tmp, &x, &y);
			if(ttt_add(ttt, ID_P2, POINT(x,y))<0){
				winner = (id2==app1?2:1);
				printf("app%d fail due to error place(%d,%d),", id2 == app1?1:2, x,y);
				break;
			}
			record[record_cnt++] = POINT(x,y);
			r = ttt_isMFinish(ttt);
			if(r == ID_P2){
				winner = (id2==app1?1:2);
				printf("app%d win,", id2 == app1?1:2);
				break;
			}else if(r == DRAW){
				winner = 0;
				break;
			}
			set_m_field(ttt, m_field, POINT(x,y));
		}
		if(winner == 1){
			score[0]++;
		}else if(winner == 2){
			score[2]++;
		}else if(winner == 0){
			score[1]++;
		}
		printf("\x1b[48;5;1m%d : %d : %d\x1b[48;5;16m\n", score[0], score[1], score[2]);
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
		//dump_field(ttt);
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
