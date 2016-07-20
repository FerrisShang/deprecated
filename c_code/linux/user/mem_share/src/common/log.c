#include <stdarg.h>
#include "log.h"

void log_v(const char* fmt, ...);
void log_d(const char* fmt, ...);
void log_e(const char* fmt, ...);

struct log Log = {
	log_v,
	log_d,
	log_e,
};

void log_v(const char* fmt, ...)
{
	va_list args;
	printf("V:");
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	printf("\n");
}
void log_d(const char* fmt, ...)
{
	va_list args;
	printf("D:");
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	printf("\n");
}
void log_e(const char* fmt, ...)
{
	va_list args;
	printf("E:");
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	printf("\n");
}

char *hex2str(void *p, int len)
{
	static unsigned char buf[1024];
	unsigned char c, *hex = p;
	int i;
	for(i=0;i<len;i++){
		c = hex[i]>>4;
		if(c > 9)
			buf[i*2] = c-10+'A';
		else
			buf[i*2] = c+'0';
		c = hex[i]&0x0F;
		if(c > 9)
			buf[i*2+1] = c-10+'A';
		else
			buf[i*2+1] = c+'0';
	}
	buf[i*2] = '\0';
	return (char*)buf;
}
