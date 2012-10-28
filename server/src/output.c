#include "output.h"

#include <tato/list.h>

//=== TatoDb ===

void tato_db_output_stats(TatoDb *this, struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "\n[DATA]\n");
	evbuffer_add_printf(buffer, "changed %s\n", this->changed ? "true" : "false");
	evbuffer_add_printf(buffer, "items %i\n", tato_tree_int_size(this->items));
	evbuffer_add_printf(buffer, "relations %i\n", tato_tree_int_size(this->relations));
	evbuffer_add_printf(buffer, "LANGS %i\n", tato_tree_str_size(this->langs));

	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(this->langs, it) {
		TatoItemLang *lang = it->value;
		evbuffer_add_printf(buffer, "LANG[\"%s\"] %i\n", lang->code, tato_tree_str_size(lang->index));
	}
}

//=== TatoRelation ===

void tato_relation_output_xml_head(TatoRelation *this, struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "<rel id=\"%i\" x=\"%i\" y=\"%i\" type=\"%i\" flags=\"%i\">\n", 
		this->id,
		this->x->id,
		this->y->id,
		this->type,
		this->flags
	);
	tato_kvlist_output_xml(this->metas, buffer, "meta");
}

void tato_relation_output_xml_foot(struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "</rel>\n");
}

void tato_relation_output_xml_bfs(TatoRelation *this, struct evbuffer *buffer, int depth, TatoRelationFilter *filter, Plugins *plugins) {
	tato_relation_output_xml_head(this, buffer);

	//Show spanning tree from x
	TatoFetcherTree *tree = tato_fetcher_tree_new();
	tato_fetcher_tree_bfs(tree, this->x, depth, filter);
	tato_fetcher_tree_output_xml(tree, buffer, plugins);

	//Show spanning tree from y
	tato_fetcher_tree_clear(tree);
	tato_fetcher_tree_bfs(tree, this->y, depth, filter);
	tato_fetcher_tree_output_xml(tree, buffer, plugins);
	tato_fetcher_tree_free(tree);

	tato_relation_output_xml_foot(buffer);
}

void tato_relation_output_xml(TatoRelation *this, struct evbuffer *buffer) {
	tato_relation_output_xml_head(this, buffer);
	tato_relation_output_xml_foot(buffer);
}

//=== TatoItem ===

void tato_item_output_xml_head(TatoItem *this, struct evbuffer *buffer) {
	char *str = (char *) evhttp_htmlescape(this->str);
	evbuffer_add_printf(buffer, "<item id=\"%i\" lang=\"%s\" str=\"%s\" flags=\"%i\">\n",
		this->id, 
		this->lang->code,
		str,
		this->flags
	);
	free(str);
	tato_kvlist_output_xml(this->metas, buffer, "meta");
}

void tato_item_output_xml_foot(struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "</item>\n");
}

void tato_item_output_xml(TatoItem *this, struct evbuffer *buffer) {
	tato_item_output_xml_head(this, buffer);
	tato_item_output_xml_foot(buffer);
}

//=== TatoItemFetcher ===

void tato_item_fetcher_output_xml(TatoItemFetcher *this, struct evbuffer *buffer, int depth, TatoRelationFilter *filter, Plugins *plugins) {
	evbuffer_add_printf(buffer, "<items>\n");

	TatoFetcherTree *tree = tato_fetcher_tree_new();
	int i;
	for (i = 0; i < this->size; i++) {
		tato_fetcher_tree_bfs(tree, this->items[i], depth, filter);
		tato_fetcher_tree_output_xml(tree, buffer, plugins);
		tato_fetcher_tree_clear(tree);
	}
	tato_fetcher_tree_free(tree);

	evbuffer_add_printf(buffer, "</items>\n");
}

//=== TatoFetcherTree ===

void tato_fetcher_tree_node_output_xml(TatoFetcherTreeNode *this, struct evbuffer *buffer, Plugins *plugins) {
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

