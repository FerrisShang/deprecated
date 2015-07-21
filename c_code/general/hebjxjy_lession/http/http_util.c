#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_util.h"
#include "djpeg_if.h"
#include "chk_code.h"

struct http_handle *create_http_handle(void)
{
	struct http_handle *hhttp;
	hhttp = malloc(sizeof(struct http_handle));
	if(hhttp == NULL)
		return NULL;
	memset(hhttp, 0, sizeof(struct http_handle));
	hhttp->buf = malloc(HTTP_BUF_LEN);
	if(hhttp->buf == NULL){
		free(hhttp);
		return NULL;
	}
	return hhttp;
}

void destory_http(struct http_handle* hhttp)
{
	if(hhttp != NULL){
		free(hhttp->buf);
		free(hhttp);
	}
}

int create_http_str(struct http_handle *hhttp, char *get, char *host,
		char *accept, char *agent, char *ref, char *accept_code,
		char *accept_lan, char *cookie, char *post, char *len,
		char *origin, char *xref, char *text)
{
	char *p = hhttp->buf;
	p[0] = 0;
	if(strlen(get)>0){
		sprintf(p, "GET %s HTTP/1.1"RETURN_STR, get);
	}else if(strlen(post)>0){
		sprintf(p, "POST %s HTTP/1.1"RETURN_STR, post);
	}else{
		hhttp->buf_len = -1;
		return hhttp->buf_len;
	}
	sprintf(p, "%sHost: %s"RETURN_STR, p, post);
	strcat(p, "Connection: keep-alive"RETURN_STR);
	if(strlen(len)>0)
		sprintf(p, "%sContent-Length: %s"RETURN_STR"Cache-Control: max-age=0"RETURN_STR, p, len);
	sprintf(p, "%sAccept: %s"RETURN_STR, p, accept);
	if(strlen(origin)>0)
		sprintf(p, "%sOrigin: %s"RETURN_STR, p, origin);
	if(strlen(xref)>0)
		sprintf(p, "%sX-Requested-With: %s"RETURN_STR, p, xref);
	sprintf(p, "%sUser-Agent: %s"RETURN_STR, p, agent);
	if(strlen(post)>0)
		strcat(p, "Content-Type: application/x-www-form-urlencoded"RETURN_STR);
	if(strlen(ref)>0)
		sprintf(p, "%sReferer: %s"RETURN_STR, p, ref);
	if(strlen(accept_code)>0)
		sprintf(p, "%sAccept-Encoding: %s"RETURN_STR, p, accept_code);
	sprintf(p, "%sAccept-Language: %s"RETURN_STR, p, accept_lan);
	if(strlen(cookie)>0)
		sprintf(p, "%sCookie: %s"RETURN_STR, p, cookie);
	strcat(p, RETURN_STR);
	if(strlen(text)>0)
		strcat(p, text);
	hhttp->buf_len = strlen(p);
	return hhttp->buf_len;
}

static int hex2num_1(char hex)
{
	if(hex>='0'&&hex<='9')
		return hex - '0';
	if(hex>='a'&&hex<='z')
		return hex - 'a' + 10;
	if(hex>='A'&&hex<='Z')
		return hex - 'A' + 10;
	return 0;
}

int is_http_recv_done(char *buf, int buf_len)
{
	const char noContent[] = "Content-Length: 0";
	const char len_str[] = "Content-Length: ";
	const char text_str[] = "Content-Type: text/html";
	const char jpeg_str[] = "Content-Type: image/jpeg";
	const char html_end[] = "</html>";

	if(strstr(buf, html_end) != NULL){
		return 1;
	}else if(strstr(buf, noContent) != NULL){
		return 1;
	}else if(strstr(buf, text_str) != NULL){
		int text_len = 0;
		char *p;
		int i;
		p = strstr(buf, len_str);
		if(p == NULL)
			return 0;
		p += strlen(len_str);
		for(i=0;i<8;i++){
			text_len *= 10;
			text_len += (*p++ - '0');
			if(*p == '\r')
				break;
		}
		p = strstr(buf, RETURN_STR RETURN_STR);
		if(p == NULL)
			return 0;
		if(text_len + (p - buf) + strlen(RETURN_STR RETURN_STR)== buf_len){
			return 1;
		}
	}else if(strstr(buf, jpeg_str) != NULL){
		char *p, *len;
		int img_len = 0;
		len = strstr(buf, RETURN_STR RETURN_STR);
		if(len == NULL){
			return 0;
		}
		len += strlen(RETURN_STR RETURN_STR);
		p = strstr(len, RETURN_STR);
		if(p == NULL||((p-len)>8)||(p<len)){
			return 0;
		}
		for(;len < p;len++){
			img_len <<= 4;
			img_len += hex2num_1(*len);
		}
		if(img_len + (p - buf) + strlen(RETURN_STR) + 7/*after jpeg file*/== buf_len){
			return 1;
		}
	}
	return 0;
}

char *set_cookie(struct http_handle *hhttp, char *buf)
{
	char *p;
	p = strstr(buf, COOKIE_PREFIX);
	if(p == NULL){
		hhttp->cookie[0] = '0';
		return NULL;
	}else{
		memcpy(hhttp->cookie, p, COOKIE_LEN);
		return p;
	}
}

char *get_check_code(struct http_handle *hhttp, char *buf, int buf_len)
{
	const char *const_return = RETURN_STR RETURN_STR;
	char *p, *len;
	int img_len = 0;
	len = strstr(buf, const_return);
	if(len == NULL){
		hhttp->check_code[0] = '0';
		return NULL;
	}
	len += strlen(RETURN_STR RETURN_STR);
	p = strstr(len, RETURN_STR);
	if(p == NULL||((p-len)>8)||(p<len)){
		hhttp->check_code[0] = '0';
		return NULL;
	}
	for(;len < p;len++){
		img_len <<= 4;
		img_len += hex2num_1(*len);
	}
	if(img_len + (p - buf) + strlen(RETURN_STR) + 7/*after jpeg file*/!= buf_len){
		hhttp->check_code[0] = '0';
		return NULL;
	}


	struct djpeg_infile infile;
	struct djpeg_outfile outfile;
	unsigned char *img_buf;

	img_buf = malloc(100*100);
	infile.buf = (unsigned char*)p + strlen(RETURN_STR);
	infile.len = img_len;
	outfile.buf = img_buf;
	if(djpeg(&infile, &outfile)<0){
		free(img_buf);
		return NULL;
	}
	if(-1 == check_code_data(img_buf, CODE_IMG_WIDTH, CODE_IMG_HEIGHT, hhttp->check_code)){
		free(img_buf);
		return NULL;
	}
	free(img_buf);
	return hhttp->check_code;
}

int is_login_suc(struct http_handle *hhttp, char *buf, int buf_len)
{
	const char suc1[] = "302 Found";
	const char suc2[] = "/jxjy/user/index.do";
	if(strstr(buf, suc1) != NULL && strstr(buf, suc2) != NULL){
		return 1;
	}else{
		return -1;
	};
}

int searchUserPlanId(struct http_handle *hhttp, char *buf, int buf_len)
{
	const char userPlanID_str[] =  "<td><a href=\"/jxjy/user/plan/userPlanInfo.do?userPlanId=";
	char *s, *e;
	s = strstr(buf, userPlanID_str);
	if(s){
		s += strlen(userPlanID_str);
		e = strstr(s, "\"");
		if(e == NULL || e-s>8)
			return -1;
		memcpy(hhttp->userPlanID, s, e-s);
		return 1;
	}else{
		return -1;
	}
}

int setCourseDone(struct http_handle *hhttp, char *buf, int buf_len)
{
	const char suc1[] = "302 Found";
	const char suc2[] = "/userCourse.do";
	if(strstr(buf, suc1) != NULL && strstr(buf, suc2) != NULL){
		return 1;
	}else{
		return -1;
	};
}

int parse_lession_name(struct http_handle *hhttp, char *buf, int buf_len)
{
	const char lession_str[] = "onclick=\"toStudy('";
	char *pbuf = buf;
	char *s, *e;
	int cn;
	do{
		s = strstr(pbuf, lession_str);
		if(s){
			s += strlen(lession_str);
			if(s - buf >= buf_len)return 1;
			e = strstr(s, "'");
			if(e == NULL || e-s>8)
				return 1;
			*e = 0;
			cn = hhttp->lession.courseNum;
			strcpy(hhttp->lession.course[cn].name, s);
			hhttp->lession.courseNum++;
			pbuf = e + 1;
			if(pbuf - buf >= buf_len)return 1;
		}else{
			return 1;
		}
	}while(1);
}

int parse_video_name(struct http_handle *hhttp, char *buf, int buf_len, int course_idx)
{
	const char lession_str[] = "onclick=\"toStudy('";
	char *pbuf = buf;
	char *s, *e, exFlag;
	int vn, i;
	do{
		s = strstr(pbuf, lession_str);
		if(s){
			s += strlen(lession_str);
			if(s - buf >= buf_len)return 1;
			e = strstr(s, "'");
			if(e == NULL || e-s>8)
				return 1;
			*e = 0;
			vn = hhttp->lession.course[course_idx].videoNum;
			exFlag = 0;
			for(i=0;i<vn;i++){
				if(!strcmp(hhttp->lession.course[course_idx].video[i].name, s)){
					exFlag = 1;
					break;
				}
			}
			if(exFlag == 0){
				printf("course(%s)-video(%s)\n", 
						hhttp->lession.course[course_idx].name,s);//////////////////////
				strcpy(hhttp->lession.course[course_idx].video[vn].name, s);
				hhttp->lession.course[course_idx].videoNum++;
			}
			pbuf = e + 1;
			if(pbuf - buf >= buf_len)return 1;
		}else{
			return 1;
		}
	}while(1);
}

int isStudyVideoDone(struct http_handle *hhttp, char *buf, int buf_len)
{
	const char suc1[] = "HTTP/1.1 200 OK";
	if(strstr(buf, suc1) != NULL){
		return 1;
	}else{
		return -1;
	};
}

int parse_exer_name(struct http_handle *hhttp, char *buf, int buf_len, int course_idx)
{
	return 1;
}
