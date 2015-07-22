#include <stdio.h>
#include <unistd.h>
#include "study.h"
#include "file_read.h"

#define CONFIG_FILE_NAME "config.ini"
#define ACCOUNT_FILE_NAME "account.txt"

int main(int argc, char *argv[]) {
	struct account *account;
	char config_buf[128];
	int account_file_line;
	int account_num, i;
	char account_file_name[32] = ACCOUNT_FILE_NAME;
	char log_file_name[64] = "log";
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
	}
	free(account);
	fputs("全部完成..\n", fp);
	printf("%s 全部完成..\n", log_file_name);
	fclose(fp);
	return 0;
}
