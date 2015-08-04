#include <unistd.h>
#include "study.h"
#include "lession.h"
#include "file_read.h"

#define TRY_TIME 8
#define return_err(err) \
do{ \
	destory_http(hhttp); \
	CloseLink(tcplink); \
	account->a_st = err; \
	return; \
}while(0)

#define RUN_FUCN(err,func, ...) \
{ \
	int relay_, res_; \
	for(relay_=0;relay_<TRY_TIME;relay_++){ \
		res_ = func(__VA_ARGS__); \
		if(res_ > 0) \
			break; \
		usleep(1000000); \
	} \
	if(res_ <= 0){ \
		return_err(err); \
	} \
}

void study(void *para)
{
	struct tcplink* tcplink = 0;
	struct http_handle *hhttp = 0;
	struct account *account = para;
	int i,j,cn,vn,en,login_num,res;
	char isOK;
	account->a_st = A_ST_RUN;
	tcplink = InitLink(SERVER_IP, SERVER_PORT);
	if(tcplink == NULL){
		printf("init link failed.\n");
		return_err(A_ST_ERR_NET);
	}
	if((hhttp = create_http_handle()) == NULL){
		printf("init http failed.\n");
		CloseLink(tcplink);
		return_err(A_ST_ERR_NET);
	}
	login_num = 0;
relogin:
	RUN_FUCN(A_ST_ERR_COOKIE, lession_init_cookie,hhttp, tcplink);
	RUN_FUCN(A_ST_ERR_CODE, lession_get_check_code,hhttp, tcplink);
	res = lession_login(hhttp, tcplink, account->id, account->pass);
	if(res <= 0){
		if(!(login_num++ == TRY_TIME)){
			goto relogin;
		}else{
			return_err(A_ST_ERR_LOGIN);
		}
	}
	RUN_FUCN(A_ST_ERR_GET_UID, lession_get_userPlanID,hhttp, tcplink);

	RUN_FUCN(A_ST_ERR_GET_SCORE, lession_get_score,hhttp, tcplink, account->score, &isOK);
	if(isOK && atof(account->score) > 19.99){
		return_err(A_ST_OK);
	}

	RUN_FUCN(A_ST_ERR_SEL_COURSE, lession_sel_courses,hhttp, tcplink, account->sel_courses);
	RUN_FUCN(A_ST_ERR_SET_LES_ID, lession_get_lessionID,hhttp, tcplink);
	for(i=0;i<hhttp->lession.courseNum;i++){
		RUN_FUCN(A_ST_ERR_GET_VIDEO, lession_get_video,hhttp, tcplink, i);
	}
	cn = hhttp->lession.courseNum;
	for(i=0;i<cn;i++){
		vn = hhttp->lession.course[i].videoNum;
		for(j=0;j<vn;j++){
			RUN_FUCN(A_ST_ERR_STUDY_VIDEO,
					lession_study_video,hhttp, tcplink, hhttp->lession.course[i].video[j].name);
		}
	}
	for(i=0;i<hhttp->lession.courseNum;i++){
		RUN_FUCN(A_ST_ERR_GET_EXER_ID, lession_get_exerID,hhttp, tcplink, i);
	}
	for(i=0;i<cn;i++){
		en = hhttp->lession.course[i].exerID_Num;
		for(j=0;j<en;j++){
			char post[512];
			char text[512];
			RUN_FUCN(A_ST_ERR_GET_EXER, lession_get_exer,hhttp, tcplink, hhttp->lession.course[i].name,
					hhttp->lession.course[i].exerID[j].name, post, text);
			RUN_FUCN(A_ST_ERR_SUBMIT_EXER, lession_submit_exer,hhttp, tcplink, hhttp->lession.course[i].name,
					hhttp->lession.course[i].exerID[j].name, post, text);
		}
	}
	RUN_FUCN(A_ST_ERR_GET_SCORE, lession_get_score,hhttp, tcplink, account->score, &isOK);
	if(isOK){
		if(atof(account->score) < 19.99)
			account->a_st = A_ST_ERR_SCORE;
		else
			account->a_st = A_ST_OK;
	}else{
		account->a_st = A_ST_ERR_COURSE;
	}
	destory_http(hhttp);
	CloseLink(tcplink);
	return;
}
