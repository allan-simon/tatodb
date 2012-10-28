#ifndef _PLUGINS_H
#define _PLUGINS_H

#include <stdbool.h>

#include <tato/list.h>

#include "plugin_common.h"

typedef struct {
	void *handle;
	PluginDef *def;
	void *data;
} Plugin;

struct TatoItem_;

Plugin *plugin_new(char const *path);
void plugin_free(Plugin *this);
char const *plugin_name(Plugin *this);
char const *plugin_version(Plugin *this);
char const *plugin_description(Plugin *this);
bool plugin_param(Plugin *this, char const *name, char const *value);
bool plugin_start(Plugin *this);
void plugin_item_join(Plugin *this, struct evbuffer *buffer, struct TatoItem_ *item);
void plugin_output_stats(Plugin *this, struct evbuffer *buffer);

typedef TatoList Plugins;

Plugins *plugins_new();
void plugins_free(Plugins *this);
bool plugins_load(Plugins *this, char const *path);
bool plugins_add(Plugins *this, Plugin *plugin);
void plugins_item_join(Plugins *this, struct evbuffer *buffer, struct TatoItem_ *item);
void plugins_output_stats(Plugins *this, struct evbuffer *buffer);

#endif
