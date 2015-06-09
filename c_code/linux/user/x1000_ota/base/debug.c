#include <stdio.h>
#include "base.h"

void dbg_print(int level, int forced, const char *fmt, ...)
{
	va_list args;
	if(DEBUG_LEVEL >= level || forced == DBG_FORCE_ON){
		switch(level){
			case DBG_INFO :
				printf("DBG_INFO : ");
				break;
			case DBG_WARNING :
				printf("DBG_WARNING : ");
				break;
			case DBG_ERROR :
				printf("DBG_ERROR : ");
				break;
			default :
				break;
		}
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}
