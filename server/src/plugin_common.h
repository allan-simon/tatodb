#ifndef _PLUGIN_COMMON_H
#define _PLUGIN_COMMON_H

#include <evhttp.h>
#include <stdbool.h>

#include <tato/tato.h>

typedef struct {
	char const *name;
	char const *description;
	char const *version;
	void *(*plugin_new)(void);
	void (*plugin_free)(void *);
	bool (*plugin_start)(void *);
	bool (*plugin_param)(void *, char const *, char const *);
	void (*plugin_item_join)(void *, struct evbuffer *, struct TatoItem_ *);
	void (*plugin_output_stats)(void *, struct evbuffer *);
} PluginDef;

#endif

