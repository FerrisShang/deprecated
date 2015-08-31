#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "study.h"
#include "file_read.h"

#define CONFIG_FILE_NAME "config.ini"
#define ACCOUNT_FILE_NAME "account.txt"

#ifndef __LINUX__

#include <windows.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int total_num;
int curr_num;
int finish_num;
#define PTHREAD_NUM 3

char account_file_name[32] = ACCOUNT_FILE_NAME;
char log_file_name[64] = "log.txt";

void output_file(int num, struct account *account)
{
	char str_buf[256];
	int i;
	FILE *fp;
	fp = fopen(log_file_name, "w");
	for(i=0;i<num;i++){
		sprintf(str_buf, "(%d/%d)\t%s\t%s\tscore: %s\n",
				i+1, num,
				account[i].id,
				status_str[(int)account[i].a_st],
				account[i].score);
		fputs(str_buf, fp);
	}
	fclose(fp);
}
void *pthread_study(void *para)
{
	int idx;
	struct account *account;
	account = para;
	while(1){
		pthread_mutex_lock(&mutex);
		if(curr_num == total_num){
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
			return;
		}
		idx = curr_num;
		curr_num++;
		pthread_mutex_unlock(&mutex);

		study((void*)&account[idx]);

		pthread_mutex_lock(&mutex);
		finish_num++;
		printf("(%d/%d)\t%s\t%s\tscore: %s\n",
				idx+1, total_num,
				account[idx].id,
				status_str[(int)account[idx].a_st],
				account[idx].score);
		output_file(curr_num, account);
		pthread_mutex_unlock(&mutex);
	}
}
int main(int argc, char *argv[]) {
	struct account *account;
	char config_buf[128];
	int account_file_line;
	int account_num, i;
	char str_buf[256];
	FILE *fp;
	pthread_t thread[PTHREAD_NUM];
	if(argc > 1){
		strcpy(account_file_name, argv[1]);
	}
	if(argc > 2){
		sprintf(log_file_name, "%s.log", argv[2]);
	}
	if(NULL == read_config(CONFIG_FILE_NAME, config_buf)){
		printf("read config failed\n");
		return 0;
	}
	account_file_line = read_line_num(account_file_name);
	if(account_file_line <= 0){
		printf("read accounts failed\n");
		return 0;
	}
	account = malloc(sizeof(struct account) * account_file_line);
	if(account == NULL){
		printf("not enough memory\n");
		return 0;
	}
	account_num = init_account(account_file_name, account, config_buf);
	printf("total:%d\n",account_num);
	if(account_num <= 0){
		printf("parse account error\n");
	}
	total_num = account_num;
	curr_num = 0;
	finish_num = 0;
	for(i=0;i<PTHREAD_NUM;i++){
		if(curr_num == total_num)
			break;
		pthread_create(&thread[i],NULL,pthread_study,(void*)account);
		sleep(2);
	}
	while(1){
		pthread_mutex_lock(&mutex);
		if(finish_num == total_num){
			pthread_mutex_unlock(&mutex);
			break;
		}else{
			pthread_mutex_unlock(&mutex);
		}
		sleep(2);
	}
	output_file(account_num, account);
	fp = fopen(log_file_name, "a+");
	fputs("学习完毕..\n", fp);
	free(account);
	fclose(fp);
	printf("信息已输出到文件：%s （按回车键关闭）\n", log_file_name);
	getchar();
	return 0;
}

#else

int main(int argc, char *argv[]) {
	struct account *account;
	char config_buf[128];
	int account_file_line;
	int account_num, i;
	char account_file_name[32] = ACCOUNT_FILE_NAME;
	char log_file_name[64] = "log.txt";
	char str_buf[256];
	FILE *fp;
	if(argc > 1){
		strcpy(account_file_name, argv[1]);
	}
	if(argc > 2){
		sprintf(log_file_name, "%s.log", argv[2]);
	}
	if(NULL == read_config(CONFIG_FILE_NAME, config_buf)){
		printf("read config failed\n");
		return 0;
	}
	account_file_line = read_line_num(account_file_name);
	if(account_file_line <= 0){
		printf("read accounts failed\n");
		return 0;
	}
	account = malloc(sizeof(struct account) * account_file_line);
	if(account == NULL){
		printf("not enough memory\n");
		return 0;
	}
	account_num = init_account(account_file_name, account, config_buf);

	fp = fopen(log_file_name, "a+");
	if(account_num <= 0){
		printf("parse account error\n");
	}else{
		sprintf(str_buf, "read %d accounts\n", account_num);
		fputs(str_buf, fp);
	}

	for(i=0;i<account_num;i++){
		study((void*)&account[i]);
		sprintf(str_buf, "(%d/%d)\t%s\t%s\tscore: %s\n",
				i+1, account_num,
				account[i].id,
				status_str[(int)account[i].a_st],
				account[i].score);
		fputs(str_buf, fp);
		fflush(fp);
		printf(str_buf);
	}
	free(account);
	fputs("学习完毕..\n", fp);
	printf("信息已输出到文件：%s （按任意键关闭）\n", log_file_name);
	fclose(fp);
	getchar();
	return 0;
}
#endif /* __LINUX */
