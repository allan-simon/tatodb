#include "plugins.h"

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include <tato/log.h>
#include <tato/item.h>

#include "plugin_common.h"

Plugin *plugin_new(char const *path) {
	void *handle = dlopen(path, RTLD_LAZY);
	if (!handle) {
		tato_log_msg_printf(LOG_ERR, "%s", dlerror());
		return NULL;
	}

	PluginDef *def = dlsym(handle, "plugin_def");
	char *error = dlerror();
	if (error) {
		tato_log_msg_printf(LOG_ERR, "%s", error);
		return NULL;
	}

	Plugin *this = malloc(sizeof(Plugin));
	this->handle = handle;
	this->def = def;
	this->data = this->def->plugin_new();
	return this;
}

void plugin_free(Plugin *this) {
	if (this->data)
		this->def->plugin_free(this->data);
	dlclose(this->handle);
	free(this);
}

bool plugin_param(Plugin *this, char const *name, char const *value) {
	return this->def->plugin_param(this->data, name, value);
}

bool plugin_start(Plugin *this) {
	return this->def->plugin_start(this->data);
}

void plugin_item_join(Plugin *this, struct evbuffer *buffer, TatoItem *item) {
	this->def->plugin_item_join(this->data, buffer, item);
}

void plugin_output_stats(Plugin *this, struct evbuffer *buffer) {
	this->def->plugin_output_stats(this->data, buffer);
}

char const *plugin_name(Plugin *this) {
	return this->def->name;	
}

char const *plugin_description(Plugin *this) {
	return this->def->description;	
}

char const *plugin_version(Plugin *this) {
	return this->def->version;	
}

Plugins *plugins_new() {
	return tato_list_new();
}

void plugins_free(Plugins *this) {
	TatoListNode *it;
	TATO_LIST_FOREACH(this, it) {
		plugin_free(it->data);
	}
	tato_list_free(this);
}

bool plugins_add(Plugins *this, Plugin *plugin) {
	tato_list_push(this, plugin);
	return true;
}

void plugins_item_join(Plugins *this, struct evbuffer *buffer, TatoItem *item) {
	TatoListNode *it;
	TATO_LIST_FOREACH(this, it) {
		plugin_item_join(it->data, buffer, item);
	}
}

void plugins_output_stats(Plugins *this, struct evbuffer *buffer) {
	int n = 0;
	TatoListNode *it;
	TATO_LIST_FOREACH(this, it) {
		Plugin *plugin = it->data;
		evbuffer_add_printf(buffer, "\n[PLUGIN #%i]\n", ++n);
		evbuffer_add_printf(buffer, "NAME \"%s\"\n", plugin_name(plugin));
		evbuffer_add_printf(buffer, "VERSION %s\n", plugin_version(plugin));
		evbuffer_add_printf(buffer, "DESCRIPTION \"%s\"\n", plugin_description(plugin));
		plugin_output_stats(plugin, buffer);
	}
}

