#ifndef _ARGS_H
#define _ARGS_H

#include <stdbool.h>

typedef struct {
	char *data;
	char *listen_host;
	int listen_port;
	char *swac_path;
	char *verbiste_path;
} Args;

Args *args_new();
void args_free(Args *this);
void args_init(Args *this);
void args_clear(Args *args);
void args_parse(Args *this, int argc, char *argv[]);

#endif
