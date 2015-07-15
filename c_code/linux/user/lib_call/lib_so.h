#ifndef __LIB_SO_H__
#define __LIB_SO_H__

#include "plugin.h"

int init_plugin(char *path, void *para);
int exit_plugin(struct plugin *plugin);

#endif /* __LIB_SO_H__ */
