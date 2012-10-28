#include "signals.h"

#include <stdlib.h>
#include <signal.h>

#include <tato/log.h>

static Http *main_http_server;

void signals_handler(int sig) {
	char const *name;
	switch (sig) {
		case SIGINT: 
			name = "SIGINT"; 
			break;
		case SIGKILL:
			name = "SIGKILL"; 
			break;
		case SIGTERM:
			name = "SIGTERM"; 
			break;
		default:
			name = "unknown";
	}	
	tato_log_msg_printf(LOG_NOTICE, "Received signal number: %i (%s)", sig, name);

	if (/*tato_db_changed_get(main_hyper_db) &&*/ main_http_server->hyperdb->default_path) {
		tato_log_msg_printf(LOG_NOTICE, "Dump data into '%s'", main_http_server->hyperdb->default_path);
		if (tato_hyper_db_dump(main_http_server->hyperdb, main_http_server->hyperdb->default_path))
			tato_log_msg_printf(LOG_NOTICE, "Dump completed");
		else
			tato_log_msg_printf(LOG_ERR, "Dump has failed!'");
	}
	http_free(main_http_server);

	exit(0);
}

void signals_handle(Http *server) {

	main_http_server = server;
	struct sigaction sa;
	sa.sa_handler = signals_handler;
	sa.sa_flags = SA_RESETHAND;

	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}
