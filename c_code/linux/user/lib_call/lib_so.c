#include <stdio.h>
#include "lib_so.h"

int init_plugin(char *path, void *para)
{
	printf("%s called\n", __func__);
	return 0;
}

int exit_plugin(struct plugin *plugin)
{
	printf("%s called\n", __func__);
	return 0;
}
