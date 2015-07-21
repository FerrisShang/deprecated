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

	//puts(tcplink->recv_buf);////////////////////


	//return success
	sty_para->ret_value = 1;
	return;
}
