#ifndef __MEM_MANAGE_H__
#define __MEM_MANAGE_H__

#include "stdlib.h"
#include "log.h"

#define __MEMORY_DEBUG__

#ifdef __MEMORY_DEBUG__
void* _mem_malloc(int size, const char *func, int line);
void* _mem_calloc(int size, int num, const char *func, int line);
void _mem_free(void *ptr, const char *func, int line);
void _mem_dump(void);

#define mem_malloc(size)       _mem_malloc ( size,       __func__, __LINE__)
#define mem_calloc(size, num)  _mem_calloc ( size, num,  __func__, __LINE__)
#define mem_calloc(size, num)  _mem_calloc ( size, num,  __func__, __LINE__)
#define mem_realloc(ptr, size) _mem_realloc( ptr,  size, __func__, __LINE__)
#define mem_free(ptr)          _mem_free   ( ptr,        __func__, __LINE__)
#define mem_dump()             _mem_dump   ()

#else

#define mem_malloc(size)         malloc(size)
#define mem_calloc(nmemb, size)  calloc(nmemb, size)
#define mem_realloc(ptr, size)   realloc(ptr, size)
#define mem_free(ptr)            free(ptr)
#define mem_dump()               do{}while(0)

#endif

#define new0(t, n) ((t*) mem_calloc((n), sizeof(t)))
#define malloc0(n) (mem_calloc((n), 1))

#endif /* __MEM_MANAGE_H__ */
