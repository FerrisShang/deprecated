#include <stdio.h>
#include <stdlib.h>
#include "base.h"
//#include "socket_if.h"
//#include "socket_udp.h"
int main(int argc, char *argv[])
{
	char *p;
	p = mem_malloc(10);
	printf("%8p\n", (void *)p);
	mem_free(p);
	dbg_print(DBG_ERROR, 1, "%s@%d\n", __func__, __LINE__);
	exit(0);
}
