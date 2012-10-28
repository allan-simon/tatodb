#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <evhttp.h>

#include "plugins.h"

#include <tato/db.h>
void tato_db_output_stats(TatoDb *this, struct evbuffer *buffer);

#include <tato/relation.h>
void tato_relation_output_xml_head(TatoRelation *this, struct evbuffer *buffer);
void tato_relation_output_xml_foot(struct evbuffer *buffer);
void tato_relation_output_xml_bfs(TatoRelation *this, struct evbuffer *buffer, int depth, TatoRelationFilter *filter, Plugins *plugins);
void tato_relation_output_xml(TatoRelation *this, struct evbuffer *buffer);

#include <tato/item.h>
void tato_item_output_xml_head(TatoItem *this, struct evbuffer *buffer);
void tato_item_output_xml_foot(struct evbuffer *buffer);
void tato_item_output_xml(TatoItem *this, struct evbuffer *buffer);

#include <tato/fetcher.h>
void tato_fetcher_output_xml(TatoItemFetcher *this, struct evbuffer *buffer, int depth, TatoRelationFilter *filter, Plugins *plugins);

#include <tato/fetcher_tree.h>
void tato_fetcher_tree_node_output_xml(TatoFetcherTreeNode *this, struct evbuffer *buffer, Plugins *plugins);
void tato_fetcher_tree_output_xml(TatoFetcherTree *this, struct evbuffer *buffer, Plugins *plugins);

#include <tato/kvlist.h>
void tato_kvlist_node_output_xml(TatoKvListNode *this, struct evbuffer *buffer, char const *name);
void tato_kvlist_output_xml(TatoKvList this, struct evbuffer *buffer, char const *name);

#endif
