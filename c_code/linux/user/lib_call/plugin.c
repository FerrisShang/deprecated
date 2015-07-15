#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "plugin.h"

struct plugin* load_plugin(char *path, void *data)
{
	char *error;
	struct plugin *plugin;
	plugin = malloc(sizeof(struct plugin));
	if(plugin == NULL){
		printf("malloc error\n");
		return NULL;
	}
	plugin->data = data;
	plugin->lib_handle = dlopen(path, RTLD_LAZY);
	if (!plugin->lib_handle){
		fprintf(stderr, "%s\n", dlerror());
		return NULL;
	}
	plugin->init = dlsym(plugin->lib_handle, INIT_PLUGIN_SYMBOL);
	if ((error = dlerror()) != NULL){
		fprintf(stderr, "%s\n", error);
		free(plugin);
		return NULL;
	}
	plugin->exit = dlsym(plugin->lib_handle, EXIT_PLUGIN_SYMBOL);
	if ((error = dlerror()) != NULL){
		fprintf(stderr, "%s\n", error);
		free(plugin);
		return NULL;
	}
	return plugin;
}

int free_plugin(struct plugin *plugin)
{
	if(plugin){
		dlclose(plugin->lib_handle);
		free(plugin);
		return 0;
	}
	return -1;
}
