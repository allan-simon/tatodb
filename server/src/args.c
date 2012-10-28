#include "args.h"

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

Args *args_new() {
	Args *this = malloc(sizeof(Args));
	args_init(this);
	return this;
}

void args_free(Args *this) {
	args_clear(this);
	free(this);
}

void args_init(Args *this) {
	this->data = NULL;
	this->listen_host = NULL;
	this->listen_port = 8080;
	this->swac_path = NULL;
	this->verbiste_path = NULL;
}

void args_clear(Args *this) {
	if (this->data) free(this->data);
	if (this->listen_host) free(this->listen_host);
	if (this->swac_path) free(this->swac_path);
	if (this->verbiste_path) free(this->verbiste_path);
	args_init(this);
}

void args_default(Args *this) {
	this->listen_host = strdup("0.0.0.0");
}

#define GET_ARG(var, fct) \
	this->var = fct(optarg);

#define GET_ARG_STR(var) \
	if (this->var) free(this->var); \
	GET_ARG(var, strdup)

#define GET_ARG_INT(var) \
	GET_ARG(var, atoi)

#define GET_ARG_BOOL(var) \
	this->var = true;

void args_help() {
	printf("Usage:\n");
	printf("  " PACKAGE " [OPTION...] target1 query1 target2 query2... \n\n");
	printf(PACKAGE " Options:\n");
	printf("  -d, --data             Set path of configuration file\n");
	printf("  -h, --listen-host      Set listening host (default is 0.0.0.0)\n");
	printf("  -p, --listen-port      Set listening port (default is 8080)\n");
	printf("  -s, --swac             Set path of swac sqlite3 database\n");
	printf("  -V, --verbiste         set path of 'verbiste' data\n");
	printf("  -?, --help             Show help options and exit\n");
	printf("  -v, --version          Show the version number and exit\n");
}

void args_version() {
	printf(PACKAGE " " VERSION "\n");
}

void args_parse(Args *this, int argc, char *argv[]) {
	args_default(this);

	struct option long_options[] = {
		{ "data", 1, NULL, 'd' },
		{ "listen-host", 1, NULL, 'h' },
		{ "listen-port", 1, NULL, 'p' },
		{ "swac", 1, NULL, 's' },
		{ "verbiste", 1, NULL, 'V' },
		{ "help", 0, NULL, '?' },
		{ "version", 0, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};

	int option_index, c;
	while ((c = getopt_long(argc, argv, "d:h:p:s:V:?v", long_options, &option_index)) != -1) {
		switch (c) {
			case 'd': 
				GET_ARG_STR(data);
				break;
			case 'h':
				GET_ARG_STR(listen_host);
				break;
			case 'p':
				GET_ARG_INT(listen_port);
				break;
			case 's':
				GET_ARG_STR(swac_path);
				break;
			case 'V':
				GET_ARG_STR(verbiste_path);
				break;
			case '?':
				args_help();
				exit(0);
				break;
			case 'v':
				args_version();
				exit(0);
				break;
		}
        }
}

