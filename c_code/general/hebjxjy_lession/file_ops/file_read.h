#ifndef __FILE_OPS_H__
#define __FILE_OPS_H__

#include <stdio.h>
#include <string.h>

struct account{
	char id[32];
	char pass[16];
	char a_st;
	char score[8];
	char sel_courses[128];
};

extern char *status_str[];
char *read_config(char *file_name, char *buf);
int read_line_num(char *file_name);
int init_account(char *file_name, struct account *account, char *config_buf);

typedef enum{
	A_ST_WAIT = 0,
	A_ST_RUN,

	A_ST_ERR_NET,
	A_ST_ERR_COOKIE,
	A_ST_ERR_CODE,
	A_ST_ERR_LOGIN,
	A_ST_ERR_GET_UID,
	A_ST_ERR_SEL_COURSE,
	A_ST_ERR_SET_LES_ID,
	A_ST_ERR_GET_VIDEO,
	A_ST_ERR_STUDY_VIDEO,
	A_ST_ERR_GET_EXER_ID,
	A_ST_ERR_GET_EXER,
	A_ST_ERR_SUBMIT_EXER,
	A_ST_ERR_GET_SCORE,

	A_ST_ERR_COURSE,
	A_ST_ERR_SCORE,
	A_ST_OK,
}ret_account;


#endif /* __FILE_OPS_H__ */
