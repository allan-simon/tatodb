#include "signals.h"

#include <stdlib.h>
#include <signal.h>

#include <tato/log.h>

static TatoDb *main_db;

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

	if (tato_db_changed_get(main_db) && main_db->default_path) {
		tato_log_msg_printf(LOG_NOTICE, "Dump data into '%s'", main_db->default_path);
		if (tato_db_dump(main_db, main_db->default_path))
			tato_log_msg_printf(LOG_NOTICE, "Dump completed");
		else
			tato_log_msg_printf(LOG_ERR, "Dump has failed!'");
	}

	exit(0);
}

void signals_handle(TatoDb *db) {
	main_db = db;
	struct sigaction sa;
	sa.sa_handler = signals_handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}
