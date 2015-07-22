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
	if(NULL == read_config(CONFIG_FILE_NAME, config_buf)){
		printf("read config failed\n");
		return 0;
	}
	account_file_line = read_line_num(ACCOUNT_FILE_NAME);
	if(account_file_line <= 0){
		printf("read accounts failed\n");
		return 0;
	}
	account = malloc(sizeof(struct account) * account_file_line);
	if(account == NULL){
		printf("not enough memory\n");
		return 0;
	}
	account_num = init_account(ACCOUNT_FILE_NAME, account, config_buf);
	if(account_num <= 0){
		printf("parse account error\n");
	}else{
		printf("read %d accounts\n", account_num);
	}

	for(i=0;i<account_num;i++){
		study((void*)&account[i]);
		printf("(%d/%d)\t%s\t%s\tscore: %s\n",
				i+1, account_num,
				account[i].id,
				status_str[(int)account[i].a_st],
				account[i].score);
	}
	free(account);
	printf("全部完成..\n");
	//system("pause");
	while(1) usleep(10000000);
	return 0;
}
