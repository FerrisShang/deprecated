#include "lession.h"

//Init cookie
int lession_init_cookie(struct http_handle *hhttp, struct tcplink *tcplink)
{
	int res;
	create_http_str(hhttp,
			"/jxjy/login.do",
			SERVER_IP,
			"*/*",
			AGENT_ARR,
			"",
			"",
			"zh-CN,zh;q=0.8,en;q=0.6",
			"","","","","","");
	//receive cookie
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("receive cookie failed\n");
		return -1;
	}
	if(set_cookie(hhttp, tcplink->recv_buf)==NULL){
		printf("find cookie failed\n");
		return -1;
	}
	return 1;
}

//get check code
int lession_get_check_code(struct http_handle *hhttp, struct tcplink *tcplink)
{
	int res;
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	create_http_str(hhttp,
			"/jxjy/validate/check_code.do",
			SERVER_IP,
			"*/*",
			AGENT_ARR,
			"http://"SERVER_IP"/jxjy/login.do",
			"gzip,deflate,sdch",
			"zh-CN,zh;q=0.8,en;q=0.6",
			hhttp->cookie,
			"","","","","");
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("receive check code failed\n");
		return -1;
	}
	if(get_check_code(hhttp, tcplink->recv_buf, tcplink->recv_len)==NULL){
		printf("get check code failed\n");
		return -1;
	}
	return 1;
}

//login
int lession_login(struct http_handle *hhttp, struct tcplink *tcplink, char *name, char *pass)
{
	char post_text[80];
	char len_str[9];
	int res;
	sprintf(post_text, "loginName=%s&password=%s&code=%s&x=0&y=0", name, pass, hhttp->check_code);
	sprintf(len_str, "%d", (int)strlen(post_text));
	create_http_str(hhttp,
			"",
			SERVER_IP,
			"*/*",
			AGENT_ARR,
			"http://"SERVER_IP"/jxjy/login.do",
			"",
			"zh-CN,zh;q=0.8,en;q=0.6",
			hhttp->cookie,
			"/jxjy/user/checkLogin.do",
			len_str,
			"","",
			post_text);
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("receive check code failed\n");
		return -1;
	}
	res = is_login_suc(hhttp, tcplink->recv_buf, tcplink->recv_len);
	if(res <= 0){
		printf("login failed\n");
		return -1;
	}
	return 1;
}

//get user id
int lession_get_userPlanID(struct http_handle *hhttp, struct tcplink *tcplink)
{
	int res;
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	create_http_str(hhttp,
			"/jxjy/user/right.do",
			SERVER_IP,
			"*/*",
			AGENT_ARR,
			"http://"SERVER_IP"/jxjy/user/index.do",
			"",
			"zh-CN,zh;q=0.8,en;q=0.6",
			hhttp->cookie,
			"","","","","");
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("get userPlanID page failed\n");
		return -1;
	}
	res = searchUserPlanId(hhttp, tcplink->recv_buf, tcplink->recv_len);
	if(res <= 0){
		printf("search userPlanID failed\n");
		return -1;
	}
	return 1;
}

//select courses
int lession_sel_courses(struct http_handle *hhttp, struct tcplink *tcplink, char *courses_str)
{
	int res;
	char ref_buf[256];
	char post_buf[256];
	char len_str[8];
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	sprintf(ref_buf, "http://"SERVER_IP"/jxjy/user/plan/addUserCoursePage.do?userPlanId=%s",hhttp->userPlanID);
	sprintf(post_buf, "/jxjy/user/plan/addUserCourse.do?userPlanType=&userPlanId=%s",hhttp->userPlanID);
	sprintf(len_str, "%d", (int)strlen(courses_str));
	create_http_str(hhttp,
			"",
			SERVER_IP,
			"*/*",
			AGENT_ARR,
			ref_buf,
			"",
			"zh-CN,zh;q=0.8,en;q=0.6",
			hhttp->cookie,
			post_buf,
			len_str,
			"http://"SERVER_IP,
			"",
			courses_str);
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("get select courses respones failed\n");
		return -1;
	}
	res = setCourseDone(hhttp, tcplink->recv_buf, tcplink->recv_len);
	if(res <= 0){
		printf("select courses failed\n");
		return -1;
	}
	return 1;
}

//get lession IDs
int lession_get_lessionID(struct http_handle *hhttp, struct tcplink *tcplink)
{
	int res;
	char get_buf[256];
	char ref_buf[256];
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	sprintf(get_buf, "/jxjy/user/plan/userPlanInfo.do?userPlanId=%s",hhttp->userPlanID);
	sprintf(ref_buf, "http://"SERVER_IP"/jxjy/user/right.do");
	create_http_str(hhttp,
			get_buf,
			SERVER_IP,
			"*/*",
			AGENT_ARR,
			ref_buf,
			"",
			"zh-CN,zh;q=0.8,en;q=0.6",
			hhttp->cookie,
			"", "", "", "", "");
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("get lessionID respones failed\n");
		return -1;
	}
	res = parse_lession_name(hhttp, tcplink->recv_buf, tcplink->recv_len);
	printf("get %d lessions, ", hhttp->lession.courseNum);
	if(res <= 0){
		printf("get lessionID failed\n");
		return -1;
	}
	return 1;
}
