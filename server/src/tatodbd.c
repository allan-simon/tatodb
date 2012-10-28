#include <stdlib.h>

#include "args.h"
#include "http.h"
#include "signals.h"
#include "plugins.h"

int main(int argc, char *argv[]) {
	Args *args = args_new();
	args_parse(args, argc, argv);

	//load plugins
	Plugins *plugins = plugins_new();

	//swac plugin
	if (args->swac_path) {
		Plugin *plugin = plugin_new("/usr/local/lib/tatodb/libswac.so");
		if (plugin) {
			plugin_param(plugin, "path", args->swac_path);
			if (plugin_start(plugin))
				plugins_add(plugins, plugin);
		}
	}

	//verbiste plugin
	if (args->verbiste_path) {
		Plugin *plugin = plugin_new("/usr/local/lib/tatodb/libverbiste.so");
		if (plugin) {
			plugin_param(plugin, "path", args->verbiste_path);
			if (plugin_start(plugin))
				plugins_add(plugins, plugin);
		}
	}

	//init data
	TatoDb *db = tato_db_new();
	if (args->data)
		tato_db_load(db, args->data);

	//handle system signals
	signals_handle(db);

	//start server
	Http *http = http_new(db, plugins);
	http_start(http, args->listen_host, args->listen_port);

	//free data
	http_free(http);
	tato_db_free(db);
	plugins_free(plugins);
	args_free(args);
	return 0;
}
