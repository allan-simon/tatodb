#ifndef _TATO_FETCHER_TREE_H
#define _TATO_FETCHER_TREE_H

#include <stdbool.h>

#include "item.h"
#include "relation.h"
#include "tree_int.h"
#include "list.h"

typedef struct TatoFetcherTreeNode_ {
	TatoItem *item;
	TatoRelation *relation;
	TatoList *childs;
	int depth;
} TatoFetcherTreeNode;

TatoFetcherTreeNode *tato_fetcher_tree_node_new(
	TatoItem *item,
	TatoRelation *relation
);
void tato_fetcher_tree_node_add_child(
	TatoFetcherTreeNode *thiss,
	TatoFetcherTreeNode *item
);
void tato_fetcher_tree_node_free(TatoFetcherTreeNode *thiss);

typedef struct TatoFetcherTree_ {
	TatoTreeInt *tree;
	TatoFetcherTreeNode *root;
} TatoFetcherTree;

TatoFetcherTree *tato_fetcher_tree_new();
void tato_fetcher_tree_free(TatoFetcherTree *thiss);
void tato_fetcher_tree_clear(TatoFetcherTree *thiss);
bool tato_fetcher_tree_add(
	TatoFetcherTree *thiss,
	TatoFetcherTreeNode *parent,
	TatoFetcherTreeNode *child
);

void tato_fetcher_tree_bfs(
	TatoFetcherTree *thiss,
	TatoItem *root,
	int depth,
	TatoRelationFilter *filter
);

#endif
