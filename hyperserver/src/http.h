#ifndef _HTTP_H
#define _HTTP_H

#include <stdbool.h>
#include <time.h>

#include <tato/hyperdb.h>
#include "args.h"

#include "plugins.h"

typedef struct {
    unsigned int hyper_item_add_mutal_translation_relation;
	unsigned int hyper_item_get;
	unsigned int hyper_item_rand;
	unsigned int hyper_item_add;
	unsigned int hyper_item_update;
	unsigned int hyper_item_delete;
	unsigned int hyper_item_merge;
	unsigned int hyper_item_meta_set;
	unsigned int hyper_item_meta_unset;
	unsigned int hyper_items_get;
	unsigned int hyper_items_rand;
	unsigned int hyper_items_related;
	unsigned int hyper_items_search;
	unsigned int hyper_items_suggest;
	unsigned int hyper_relation_get;
	unsigned int hyper_relation_add;
	unsigned int hyper_relation_update;
	unsigned int hyper_relation_delete;
	unsigned int hyper_relation_meta_set;
	unsigned int hyper_relation_meta_unset;
	unsigned int dump;
	unsigned int stats;
} HttpStats;

typedef struct {
	bool embeded_index;
	HttpStats stats;
	struct evhttp *server;
	struct event_base *base;
	Args *args;
	time_t started_at;
	TatoHyperDb *hyperdb;
	Plugins *plugins;
} Http;

Http *http_new(TatoHyperDb *hyperdb, Plugins *plugins, Args *args);
void http_clear_stats(Http *this);
void http_free(Http *this);
void http_start(Http *this, char const *addr, int port);

#endif
