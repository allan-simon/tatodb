#ifndef _HTTP_H
#define _HTTP_H

#include <stdbool.h>
#include <time.h>

#include <tato/db.h>

#include "plugins.h"

typedef struct {
	unsigned int item_get;
	unsigned int item_rand;
	unsigned int item_add;
	unsigned int item_update;
	unsigned int item_delete;
	unsigned int item_merge;
	unsigned int item_meta_set;
	unsigned int item_meta_unset;
	unsigned int items_get;
	unsigned int items_rand;
	unsigned int items_related;
	unsigned int items_search;
	unsigned int items_suggest;
	unsigned int relation_get;
	unsigned int relation_add;
	unsigned int relation_update;
	unsigned int relation_delete;
	unsigned int relation_meta_set;
	unsigned int relation_meta_unset;
	unsigned int dump;
	unsigned int stats;
} HttpStats;

typedef struct {
	bool embeded_index;
	HttpStats stats;
	time_t started_at;
	TatoDb *db;
	Plugins *plugins;
} Http;

Http *http_new(TatoDb *db, Plugins *plugins);
void http_clear_stats(Http *this);
void http_free(Http *this);
void http_start(Http *this, char const *addr, int port);

#endif
