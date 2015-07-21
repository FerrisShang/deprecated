#ifndef __LESSION_H__
#define __LESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "djpeg_if.h"
#include "chk_code.h"
#include "TcpLink.h"
#include "tcp_linux.h"
#include "http_util.h"

int lession_init_cookie(struct http_handle *hhttp, struct tcplink *tcplink);
int lession_get_check_code(struct http_handle *hhttp, struct tcplink *tcplink);
int lession_login(struct http_handle *hhttp, struct tcplink *tcplink,
		char *name, char *pass);
int lession_get_userPlanID(struct http_handle *hhttp, struct tcplink *tcplink);
int lession_sel_courses(struct http_handle *hhttp, struct tcplink *tcplink, char *courses_str);
int lession_get_lessionID(struct http_handle *hhttp, struct tcplink *tcplink);
int lession_get_video(struct http_handle *hhttp, struct tcplink *tcplink, int course_idx);
int lession_study_video(struct http_handle *hhttp, struct tcplink *tcplink, char *video_id);
int lession_get_exer(struct http_handle *hhttp, struct tcplink *tcplink, int course_idx);

#endif /* __LESSION_H__ */
