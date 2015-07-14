#ifndef __MEM_MANAGE_H__
#define __MEM_MANAGE_H__

#include "base.h"
#define mem_malloc(size) malloc(size)
#define mem_free(ptr)	 \
do						 \
{						 \
	free(ptr);			 \
	ptr = NULL;			 \
}while(0)

#endif /* __MEM_MANAGE_H__ */
