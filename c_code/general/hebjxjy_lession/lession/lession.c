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

