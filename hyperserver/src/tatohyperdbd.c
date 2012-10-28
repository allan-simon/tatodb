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

	//init data
	TatoHyperDb *hyperdb = tato_hyper_db_new();
	if (args->data)
		tato_hyper_db_load(hyperdb, args->data);


	//start server
	Http *http = http_new(hyperdb, plugins, args);

	//handle system signals
	signals_handle(http);

	http_start(http, args->listen_host, args->listen_port);



	//free data
	if (http) http_free(http);
	return 0;
}
