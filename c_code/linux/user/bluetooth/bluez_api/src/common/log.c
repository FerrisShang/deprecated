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
