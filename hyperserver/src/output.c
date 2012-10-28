#include "output.h"

#include <tato/list.h>

//=== TatoHyperDb ===

void tato_hyper_db_output_stats(TatoHyperDb *this, struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "\n[DATA]\n");
	evbuffer_add_printf(buffer, "changed %s\n", this->changed ? "true" : "false");
	evbuffer_add_printf(buffer, "items %i\n", tato_tree_int_size(this->items));
	evbuffer_add_printf(buffer, "relations %i\n", tato_tree_int_size(this->relations));
	evbuffer_add_printf(buffer, "LANGS %i\n", tato_tree_str_size(this->langs));

	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(this->langs, it) {
		TatoHyperItemLang *lang = it->value;
		evbuffer_add_printf(buffer, "LANG[\"%s\"] %i\n", lang->code, tato_tree_str_size(lang->index));
	}
}

//=== TatoHyperRelation ===

void tato_hyper_relation_output_xml_head(
	TatoHyperRelation *this,
	struct evbuffer *buffer
) {
	evbuffer_add_printf(
		buffer,
		"<hyperrel id=\"%i\" start=\"%i\" type=\"%i\" flags=\"%i\">\n", 
		this->id,
		this->start->id,
		this->type,
		this->flags
	);
	//TODO add output of this->ends (linked list)
	tato_kvlist_output_xml(this->metas, buffer, "meta");
}

void tato_hyper_relation_output_xml_foot(struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "</hyperrel>\n");
}

void tato_hyper_relation_output_xml_bfs(
	TatoHyperRelation *this,
	struct evbuffer *buffer,
	int depth,
	TatoHyperRelationFilter *filter,
	Plugins *plugins
) {
	tato_hyper_relation_output_xml_head(this, buffer);

	//Show spanning tree from start
	/*
	TatoFetcherTree *tree = tato_fetcher_tree_new();
	tato_fetcher_tree_bfs(tree, this->start, depth, filter);
	tato_fetcher_tree_output_xml(tree, buffer, plugins);
	*/

	tato_hyper_relation_output_xml_foot(buffer);
}

/**
 * Like BFS but without checking if a node is already present on an
 * other branch
 */
void tato_hyper_relation_output_xml_tree(
	TatoHyperRelation *this,
	struct evbuffer *buffer,
	int depth,
	TatoHyperRelationFilter *filter,
	Plugins *plugins
) {
	tato_hyper_relation_output_xml_head(this, buffer);

	//Show spanning tree from start
	/*
	tato_fetcher_tree_bfs(tree, this->start, depth, filter);
	tato_fetcher_tree_output_xml(tree, buffer, plugins);
	*/
	TatoHyperItemsNode *it;
	TATO_HYPER_ITEMS_FOREACH(this->ends, it) {
		tato_hyper_item_output_xml_head(it->item, buffer);
		tato_hyper_item_output_xml_foot(buffer);
	}

	tato_hyper_relation_output_xml_foot(buffer);
}





void tato_hyper_relation_output_xml(TatoHyperRelation *this, struct evbuffer *buffer) {
	tato_hyper_relation_output_xml_head(this, buffer);
	tato_hyper_relation_output_xml_foot(buffer);
}

//=== TatoHyperItem ===

void tato_hyper_item_output_xml_head(TatoHyperItem *this, struct evbuffer *buffer) {
	char *str = (char *) evhttp_htmlescape(this->str);
	evbuffer_add_printf(buffer, "<hyperitem id=\"%i\" lang=\"%s\" str=\"%s\" flags=\"%i\">\n",
		this->id, 
		this->lang->code,
		str,
		this->flags
	);
	free(str);
	tato_kvlist_output_xml(this->metas, buffer, "meta");
}

void tato_hyper_item_output_xml_foot(struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "</hyperitem>\n");
}

void tato_hyper_item_output_xml(TatoHyperItem *this, struct evbuffer *buffer) {
	tato_hyper_item_output_xml_head(this, buffer);
	tato_hyper_item_output_xml_foot(buffer);
}


void tato_hyper_item_output_xml_tree(TatoHyperItem *this, struct evbuffer *buffer) {
	tato_hyper_item_output_xml_head(this, buffer);
	
	TatoHyperRelationsNode *it;
	TATO_HYPER_RELATIONS_FOREACH(this->endofs, it) {
		tato_hyper_relation_output_xml_head(it->relation, buffer);

		TatoHyperItemsNode *it2;
		TATO_HYPER_ITEMS_FOREACH(it->relation->ends, it2) {
			tato_hyper_item_output_xml_head(it2->item, buffer);
			tato_hyper_item_output_xml_foot(buffer);
		}

		tato_hyper_relation_output_xml_foot(buffer);
	}

	tato_hyper_item_output_xml_foot(buffer);
}




//=== TatoHyperItemFetcher ===

void tato_hyper_item_fetcher_output_xml(TatoHyperItemFetcher *this, struct evbuffer *buffer, int depth, TatoRelationFilter *filter, Plugins *plugins) {
	evbuffer_add_printf(buffer, "<items>\n");

	int i;
	for (i = 0; i < this->size; i++) {
		tato_hyper_item_output_xml_tree(this->items[i], buffer);
	}

	evbuffer_add_printf(buffer, "</items>\n");
}

//=== TatoFetcherTree ===

void tato_fetcher_tree_node_output_xml(TatoFetcherTreeNode *this, struct evbuffer *buffer, Plugins *plugins) {
	/*
	tato_item_output_xml_head(this->item, buffer);
	plugins_item_join(plugins, buffer, this->item);

	TatoListNode *it;
	TATO_LIST_FOREACH(this->childs, it) {
		TatoFetcherTreeNode *node = it->data;
		tato_relation_output_xml_head(node->relation, buffer);
		tato_fetcher_tree_node_output_xml(node, buffer, plugins);
		tato_relation_output_xml_foot(buffer);
	}

	tato_item_output_xml_foot(buffer);
	*/
}

void tato_fetcher_tree_output_xml(TatoFetcherTree *this, struct evbuffer *buffer, Plugins *plugins) {
	tato_fetcher_tree_node_output_xml(this->root, buffer, plugins);
}

//=== TatoKvList ===

void tato_kvlist_node_output_xml(TatoKvListNode *this, struct evbuffer *buffer, char const *name) {
	char *key = (char *) evhttp_htmlescape(this->key);
	char *value = (char *) evhttp_htmlescape(this->value);
	evbuffer_add_printf(buffer, "<%s key=\"%s\" value=\"%s\"/>\n", name, key, value);
	free(key);
	free(value);
}

void tato_kvlist_output_xml(TatoKvList this, struct evbuffer *buffer, char const *name) {
	TatoKvListNode *it;
	TATO_KVLIST_FOREACH(this, it) {
		tato_kvlist_node_output_xml(it, buffer, name);
	}
}

