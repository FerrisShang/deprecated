#include <stdio.h>
#include "io.h"

void io_printf(const char *fmt, ...)
{
	va_list ap; 
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fflush(stdout);
	va_end(ap); 
}
void dbg_printf(const char *fmt, ...)
{
	va_list ap; 
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fflush(stderr);
	va_end(ap); 
}
