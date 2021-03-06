#ifndef __LOG_H__
#define __LOG_H__
#include <stdio.h>

struct log {
	void (*v)(const char* fmt, ...);
	void (*d)(const char* fmt, ...);
	void (*e)(const char* fmt, ...);
};
extern struct log Log;
char *hex2str(void *p, int len);

#endif /* __LOG_H__ */
