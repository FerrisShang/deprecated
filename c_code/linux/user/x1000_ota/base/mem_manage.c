#include <stdlib.h>
#include "base.h"

void *mem_malloc(u32_t size)
{
	return malloc(size);
}

void mem_free(void *ptr)
{
	free(ptr);
}
