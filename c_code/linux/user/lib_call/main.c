#include <stdio.h>
#include "plugin.h"

int main(int argc, char **argv) 
{
	struct plugin *plugin;
	plugin = load_plugin("./lib_so.so", (void*)0x12345678);
	if(plugin == NULL){
		printf("load plugin error\n");
		return -1;
	}
	(*(plugin->init))(plugin);
	(*(plugin->exit))(plugin);
	printf("plugin->data@0x%08x\n", (int)(plugin->data));
	free_plugin(plugin);
	return 0;
}
