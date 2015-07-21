#ifndef __STUDY_H__
#define __STUDY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TcpLink.h"
#include "tcp_linux.h"
#include "http_util.h"

struct study_para{
	char name[24];
	char pass[16];
	char sel_courses[128];
	int ret_value;
};

void study(void *para);

#endif /* __STUDY_H__ */
