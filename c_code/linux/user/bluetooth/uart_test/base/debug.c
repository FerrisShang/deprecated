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

void dbg_hex(s8_t *msg, s8_t *buf, u32_t len)
{
	int i;
	printf("%s(len=%d):", msg, len);
	for(i=0;i<len;i++){
		if((i%16)==0)
			printf("\n   ");
		printf("%02x ", (u8_t)buf[i]);
	}
	printf("\n");
}
