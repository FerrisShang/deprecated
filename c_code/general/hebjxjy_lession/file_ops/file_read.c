#include "file_read.h"

char *read_config(char *file_name, char *buf)
{
	FILE *fp;
	int len;
	fp = fopen(file_name, "r");
	if(fp == NULL){
		return NULL;
	}
	if(fgets(buf, 256, fp) == NULL){
		return NULL;
	}
	fclose(fp);
	len = (int)strlen(buf);
	while(len>0&&!(buf[len-1]>32&&buf[len-1]<127)){
		buf[len-1] = '\0';
		len--;
	}
	if(len < 4)
		return NULL;
	return buf;
}

int read_line_num(char *file_name)
{
	FILE *fp;
	char buf[1024];
	int num = 0;
	fp = fopen(file_name, "r");
	if(fp == NULL){
		return -1;
	}
	while(NULL != fgets(buf, 1024, fp)){
		num++;
	}
	fclose(fp);
	return num;
}

int init_one_account(struct account *account, char *id_buf)
{
	char *ch;
	ch = id_buf;
	while(*ch<=32 || *ch>=127){
		if(*ch == '\0'){
			return -1;
		}
		ch++;
	}
	id_buf = ch;
	account->pass[0] = '\0';
	account->a_st = A_ST_WAIT;
	while(*ch>32 && *ch<127){
		ch++;
	}
	if(*ch == '\0'){
		strcpy(account->id, id_buf);
		strcpy(account->pass, &account->id[strlen(account->id)-6]);
		return 1;
	}else{
		*ch = '\0';
		strcpy(account->id, id_buf);
		ch++;
		id_buf = ch;
		while(*ch<=32 || *ch>=127){
			if(*ch == '\0'){
				strcpy(account->pass, &account->id[strlen(account->id)-6]);
				return 1;
			}
			ch++;
		}
		id_buf = ch;
		while(*ch>32 && *ch<127){
			ch++;
		}
		*ch = '\0';
		strcpy(account->pass, id_buf);
		return 1;
	}
}

int init_account(char *file_name, struct account *account, char *config_buf)
{
	char buf[1024];
	int acc_num, res;
	FILE *fp;
	fp = fopen(file_name, "r");
	if(fp == NULL){
		return -1;
	}
	acc_num = 0;
	while((fgets(buf, 1024, fp) != NULL)){
		res = init_one_account(&account[acc_num], buf);
		if(res <= 0){
			continue;
		}
		strcpy(account[acc_num].sel_courses, config_buf);
		strcpy(account[acc_num].score, " -- ");
		acc_num++;
	}
	fclose(fp);
	return acc_num;
}

char *status_str[] = {
	[A_ST_WAIT]            = "���ڵȴ�      ",
	[A_ST_RUN]             = "����ѧϰ      ",
	[A_ST_ERR_NET]         = "�����ʼ���쳣",
	[A_ST_ERR_COOKIE]      = "��ȡcookieʧ��",
	[A_ST_ERR_CODE]        = "��ȡ��֤��ʧ��",
	[A_ST_ERR_LOGIN]       = "�˻���½ʧ��  ",
	[A_ST_ERR_GET_UID]     = "ID�쳣���ѿ���",
	[A_ST_ERR_SEL_COURSE]  = "ѡ��ʧ��      ",
	[A_ST_ERR_SET_LES_ID]  = "��ȡ�γ�IDʧ��",
	[A_ST_ERR_GET_VIDEO]   = "��ȡ��Ƶʧ��  ",
	[A_ST_ERR_STUDY_VIDEO] = "��Ƶѧϰʧ��  ",
	[A_ST_ERR_GET_EXER_ID] = "��ȡϰ��IDʧ��",
	[A_ST_ERR_GET_EXER]    = "��ȡϰ��ʧ��  ",
	[A_ST_ERR_SUBMIT_EXER] = "�ύϰ��ʧ��  ",
	[A_ST_ERR_GET_SCORE]   = "��ȡ����ʧ��  ",

	[A_ST_ERR_COURSE]      = "ѡ�η�������  ",
	[A_ST_ERR_SCORE]       = "��ѧ��������  ",
	[A_ST_OK]              = "���          ",
};
