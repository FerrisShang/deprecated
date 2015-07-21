#include "study.h"
#include "lession.h"


#define return_err() \
do{ \
	destory_http(hhttp); \
	CloseLink(tcplink); \
	sty_para->ret_value = -1; \
	return; \
}while(0)

void study(void *para)//return handle not implementation yet
{
	struct tcplink* tcplink = 0;
	struct http_handle *hhttp = 0;
	struct study_para *sty_para = para;
	int i,j,cn,vn;

	tcplink = InitLink(SERVER_IP, SERVER_PORT);
	if(tcplink == NULL){
		printf("init link failed.\n");
		return_err();
	}
	if((hhttp = create_http_handle()) == NULL){
		printf("init http failed.\n");
		CloseLink(tcplink);
		return_err();
	}
	//not add err judge function yet
	lession_init_cookie(hhttp, tcplink);
		puts("lession_init_cookie done.");////////////////////////////////////////
	lession_get_check_code(hhttp, tcplink);
		puts("lession_get_check_code done");////////////////////////////////////////
	lession_login(hhttp, tcplink, sty_para->name, sty_para->pass);
		puts("lession_login done");////////////////////////////////////////
	lession_get_userPlanID(hhttp, tcplink);
		puts("lession_get_userPlanID done");////////////////////////////////////////
	lession_sel_courses(hhttp, tcplink, sty_para->sel_courses);
		puts("lession_sel_courses done");////////////////////////////////////////
	lession_get_lessionID(hhttp, tcplink);
		puts("get lession id done");////////////////////////////////////////
	for(i=0;i<hhttp->lession.courseNum;i++){
		lession_get_video(hhttp, tcplink, i);
	}
		puts("get video done");////////////////////////////////////////
	cn = hhttp->lession.courseNum;
	for(i=0;i<cn;i++){
		vn = hhttp->lession.course[i].videoNum;
		for(j=0;j<vn;j++){
			lession_study_video(hhttp, tcplink, hhttp->lession.course[i].video[j].name);
		}
	}
		puts("study video done");////////////////////////////////////////
	for(i=0;i<hhttp->lession.courseNum;i++){
		lession_get_exer(hhttp, tcplink, i);
	}
		puts("get exercise done");////////////////////////////////////////

	//puts(tcplink->recv_buf);////////////////////
	//return success
	sty_para->ret_value = 1;
	return;
}
