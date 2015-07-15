#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#define INIT_PLUGIN_SYMBOL "init_plugin"
#define EXIT_PLUGIN_SYMBOL "exit_plugin"

struct plugin;
typedef int (*init_plugin_def)(struct plugin *plugin);
typedef int (*exit_plugin_def)(struct plugin *plugin);

struct plugin{
	void *lib_handle;
	init_plugin_def init;
	exit_plugin_def exit;
	void *data;
};

struct plugin* load_plugin(char *path, void *data);
int free_plugin(struct plugin *plugin);

#endif /* __PLUGIN_H__ */
