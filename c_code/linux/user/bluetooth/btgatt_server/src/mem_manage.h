#ifndef __MEM_MANAGE_H__
#define __MEM_MANAGE_H__

#include "stdlib.h"

#if 0
void* _mem_malloc(int size, const char *func, int line);
void* _mem_calloc(int size, int num, const char *func, int line);
void _mem_free(void *ptr, const char *func, int line);
void mem_free_cb(void *ptr);
void mem_dump(void);

#define mem_malloc(size)       _mem_malloc(size, __func__, __LINE__)
#define mem_calloc(size, num)  _mem_calloc(size, num, __func__, __LINE__)
#define mem_free(ptr)          _mem_free(ptr, __func__, __LINE__)

#else

#define mem_malloc(size)       malloc(size)
#define mem_calloc(size, num)  calloc(size, num)
#define mem_free(ptr)          free(ptr)
#define mem_free_cb            free
#define mem_dump(para)

#endif

#endif /* __MEM_MANAGE_H__ */
