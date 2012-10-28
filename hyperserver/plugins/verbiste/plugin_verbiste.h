#ifndef _PLUGIN_VERBISTE_H
#define _PLUGIN_VERBISTE_H

#include "plugin_common.h"

void *plugin_new();
void plugin_free(void *this);
bool plugin_param(void *this, char const *name, char const *value);
bool plugin_start(void *this);
void plugin_item_join(void *this, struct evbuffer *buffer, struct TatoItem_ *item);
void plugin_output_stats(void *this, struct evbuffer *buffer);

#endif
