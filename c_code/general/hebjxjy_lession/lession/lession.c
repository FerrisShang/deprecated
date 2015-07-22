#include "lession.h"

#ifdef __LINUX__
#include <time.h>
static void get_gmt_time(char *time_buf)
{
	//temp:  Wed%20Jul%2022%202015%2005:24:18%20GMT+0800%20(CST)
	char *m[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct"};
	char *w[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep); /*取得当地时间*/
	sprintf(time_buf, "%s%%20%s%%20%02d%%20%d%%20%02d:%02d:%02d%%20GMT+0800%%20(CST)", 
			w[p->tm_wday], m[p->tm_mon], p->tm_mday,
			1900+p->tm_year,
			p->tm_hour, p->tm_min, p->tm_sec);
}
#else
#error get time function not implement
#endif


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
	if(res <= 0){
		printf("get lessionID failed\n");
		return -1;
	}
	printf("get %d lessions, ", hhttp->lession.courseNum);
	return 1;
}

//get video
int lession_get_video(struct http_handle *hhttp, struct tcplink *tcplink, int course_idx)
{
	int res;
	char get_buf[256];
	char ref_buf[256];
	char *les_name;
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	les_name = hhttp->lession.course[course_idx].name;
	sprintf(get_buf, "/jxjy/user/plan/userChapter.do?userCourseId=%s&userPlanType=2", les_name);
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
		printf("get video respones failed\n");
		return -1;
	}
	res = parse_video_name(hhttp, tcplink->recv_buf, tcplink->recv_len, course_idx);
	if(res <= 0){
		printf("lession:%s - get video failed\n", les_name);
		return -1;
	}
	printf("lession:%s - get %d video\n", les_name, hhttp->lession.course[course_idx].videoNum);
	return 1;
}

//study video
int lession_study_video(struct http_handle *hhttp, struct tcplink *tcplink, char *video_id)
{
	int res;
	char post_buf[256];
	char ref_buf[256];
	char len_str[8];
	char text_buf[256];
	char time_buf[256];
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	get_gmt_time(time_buf);
	sprintf(post_buf, "/jxjy/user/plan/saveUserStudy.do?%s", time_buf);
	sprintf(ref_buf, "http://"SERVER_IP"/jxjy/user/plan/toStudy.do");
	sprintf(text_buf, "studyTime=%d&userChapterId=%s&isCompleted=%s", 
			(rand()%30)+10, video_id, "1");
	sprintf(len_str, "%d", (int)strlen(text_buf));
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
			"XMLHttpRequest", 
			text_buf);
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("get video study respones failed\n");
		return -1;
	}
	res = isStudyVideoDone(hhttp, tcplink->recv_buf, tcplink->recv_len);
	if(res <= 0){
		printf("video:%s - study video failed\n", video_id);
		return -1;
	}
	return 1;
}

//get exercise
int lession_get_exerID(struct http_handle *hhttp, struct tcplink *tcplink, int course_idx)
{
	int res;
	char get_buf[256];
	char ref_buf[256];
	char *les_name;
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	les_name = hhttp->lession.course[course_idx].name;
	sprintf(get_buf, "/jxjy/user/plan/userChapter.do?userCourseId=%s&userPlanType=2", les_name);
	sprintf(ref_buf, "http://115.28.82.250/jxjy/user/plan/userPlanInfo.do?userPlanId=%s",
			hhttp->userPlanID);
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
		printf("get video respones failed\n");
		return -1;
	}
	res = parse_exer_name(hhttp, tcplink->recv_buf, tcplink->recv_len, course_idx);
	if(res <= 0){
		printf("lession:%s - get exercise failed\n", les_name);
		return -1;
	}
	printf("lession:%s - get %d exercise\n", les_name, hhttp->lession.course[course_idx].videoNum);
	return 1;
}

int lession_get_exer(struct http_handle *hhttp, struct tcplink *tcplink, 
		char* course_ID, char *exer_ID, char *post, char *text)
{
	int res;
	char get_buf[256];
	char ref_buf[256];
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	sprintf(get_buf, "/jxjy/user/courseExercise/exercise.do?ucId=%s", exer_ID);
	sprintf(ref_buf, "http://"SERVER_IP"/jxjy/user/plan/userChapter.do?userCourseId=%s&userPlanType=2", course_ID);
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
		printf("get exercise failed\n");
		return -1;
	}
	res = parse_exer_ans(hhttp, tcplink->recv_buf, tcplink->recv_len, post, text);
	if(res <= 0){
		printf("lession:%s - get answer failed\n", course_ID);
		return -1;
	}
	return 1;
}

int lession_submit_exer(struct http_handle *hhttp, struct tcplink *tcplink, 
		char* course_ID, char *exer_ID, char *post, char *text)
{
	int res;
	char ref_buf[256];
	char post_buf[512];
	char len_str[8];
	if(hhttp == NULL || tcplink == NULL)
		return -1;
	sprintf(post_buf, "/jxjy/user/courseExercise/saveUserCourseExercise.do?%s", post);
	sprintf(ref_buf, "http://"SERVER_IP"/jxjy/user/courseExercise/exercise.do?ucId=%s",exer_ID);
	sprintf(len_str, "%d", (int)strlen(text));
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
			"XMLHttpRequest",
			text
			);
	res = tcp_comm(tcplink, hhttp->buf, hhttp->buf_len, is_http_recv_done);
	if(res <= 0){
		printf("get submit failed\n");
		return -1;
	}
	res = isSubmitSuc(hhttp, tcplink->recv_buf, tcplink->recv_len);
	if(res <= 0){
		printf("exercise:%s - submit failed\n", exer_ID);
		return -1;
	}
	return 1;
}

int lession_get_score(struct http_handle *hhttp, struct tcplink *tcplink, char *score, char *isOK)
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
		printf("get score respones failed\n");
		return -1;
	}

	res = parse_score(hhttp, tcplink->recv_buf, tcplink->recv_len, score, isOK);
	if(res <= 0){
		printf("get score failed\n");
		return -1;
	}
	return 1;
}


