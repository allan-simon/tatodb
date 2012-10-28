#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <evhttp.h>

#include "plugins.h"

#include <tato/hyperdb.h>
void tato_hyper_db_output_stats(TatoHyperDb *this, struct evbuffer *buffer);

#include <tato/hyperrelation.h>
void tato_hyper_relation_output_xml_head(TatoHyperRelation *this, struct evbuffer *buffer);
void tato_hyper_relation_output_xml_foot(struct evbuffer *buffer);
void tato_hyper_relation_output_xml_bfs(
	TatoHyperRelation *this,
	struct evbuffer *buffer,
	int depth,
	TatoHyperRelationFilter *filter,
	Plugins *plugins
);
void tato_hyper_relation_output_xml(
	TatoHyperRelation *this,
	struct evbuffer *buffer
);

#include <tato/hyperitem.h>
void tato_hyper_item_output_xml_head(TatoHyperItem *this, struct evbuffer *buffer);
void tato_hyper_item_output_xml_foot(struct evbuffer *buffer);
void tato_hyper_item_output_xml(TatoHyperItem *this, struct evbuffer *buffer);

#include <tato/fetcher.h>
void tato_hyper_fetcher_output_xml(
	TatoHyperItemFetcher *this,
	struct evbuffer *buffer,
	int depth,
	TatoHyperRelationFilter *filter,
	Plugins *plugins
);

#include <tato/fetcher_tree.h>
void tato_fetcher_tree_node_output_xml(TatoFetcherTreeNode *this, struct evbuffer *buffer, Plugins *plugins);
void tato_fetcher_tree_output_xml(TatoFetcherTree *this, struct evbuffer *buffer, Plugins *plugins);

#include <tato/kvlist.h>
void tato_kvlist_node_output_xml(TatoKvListNode *this, struct evbuffer *buffer, char const *name);
void tato_kvlist_output_xml(TatoKvList this, struct evbuffer *buffer, char const *name);

#endif
