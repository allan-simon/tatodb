#include "plugin_swac.h"

#include <stdlib.h>
#include <stdio.h>

#include <tato/item.h>

#include "swac.h"
#include "tree_str.h"

PluginDef plugin_def = {
	"Swac Plugin",
	"This is a plugin that allows blabalbla...",
	"0.1",
	plugin_new,
	plugin_free,
	plugin_start,
	plugin_param,
	plugin_item_join,
	plugin_output_stats
};

void *plugin_new() {
	Swac *this = swac_new();
	return this;
}

void plugin_free(void *this) {
	swac_free(this);
}

bool plugin_param(void *this, char const *name, char const *value) {
	return swac_param(this, name, value);
}

bool plugin_start(void *this) {
	swac_start(this);
	return true;
}

void plugin_item_join(void *this, struct evbuffer *buffer, TatoItem *item) {
	Lang *lang = swac_lang_find(this, tato_item_lang_code(item));
	if (!lang) return;

	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH_SIMILAR(lang->index, it, item->str) {
		sound_output_xml(it->value, buffer);
	}
}

void plugin_output_stats(void *this, struct evbuffer *buffer) {
	swac_output_stats(this, buffer);
}

