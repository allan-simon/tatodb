#include "fetcher_tree.h"

#include <stdlib.h>

#include "relations.h"

//---- FetcherTreeNode ----

TatoFetcherTreeNode *tato_fetcher_tree_node_new(TatoItem *item, TatoRelation *relation) {
	TatoFetcherTreeNode *thiss = malloc(sizeof(TatoFetcherTreeNode));
	thiss->item = item;
	thiss->relation = relation;
	thiss->childs = tato_list_new();
	thiss->depth = 0;
	return thiss;
}

void tato_fetcher_tree_node_add_child(TatoFetcherTreeNode *thiss, TatoFetcherTreeNode *child) {
	child->depth = thiss->depth + 1;
	tato_list_push(thiss->childs, child);
}

void tato_fetcher_tree_node_free(TatoFetcherTreeNode *thiss) {
	tato_list_free(thiss->childs);
	free(thiss);
}

//---- FetcherTree ----

TatoFetcherTree *tato_fetcher_tree_new() {
	TatoFetcherTree *thiss = malloc(sizeof(TatoFetcherTree));
	thiss->tree = tato_tree_int_new(); 
	thiss->root = NULL;
	return thiss;
}

void tato_fetcher_tree_free(TatoFetcherTree *thiss) {
	TatoTreeIntNode *it;
	TATO_TREE_INT_FOREACH(thiss->tree, it) {
		tato_fetcher_tree_node_free(it->value);
	}
	tato_tree_int_free(thiss->tree);
	free(thiss);
}

void tato_fetcher_tree_clear(TatoFetcherTree *thiss) {
	TatoTreeIntNode *it;
	TATO_TREE_INT_FOREACH(thiss->tree, it) {
		tato_fetcher_tree_node_free(it->value);
	}
	tato_tree_int_clear(thiss->tree);
	thiss->root = NULL;
}

bool tato_fetcher_tree_add(TatoFetcherTree *thiss, TatoFetcherTreeNode *parent, TatoFetcherTreeNode *child) {
	if (!tato_tree_int_insert_unique(thiss->tree, child->item->id, child)) {
		tato_fetcher_tree_node_free(child);
		return false;
	}

	if (!parent) {
		thiss->root = child;
	}
	else {
		tato_fetcher_tree_node_add_child(parent, child);
	}
	return true;
}

void tato_fetcher_tree_bfs(TatoFetcherTree *thiss, TatoItem *root, int depth, TatoRelationFilter *filter) {
	if (thiss->root)
		return;

	TatoList *stack = tato_list_new();

	TatoFetcherTreeNode *pivot = tato_fetcher_tree_node_new(root, NULL);
	if (tato_fetcher_tree_add(thiss, NULL, pivot)) {
		tato_list_push(stack, pivot);
	}

	while (!tato_list_empty(stack)) {
		// the next pivot node will be either the next brother of 
		// the previous pivot node, or its first child
		// for each child of the pivot node, we add it to the tree
		// if not already visited

		pivot = (TatoFetcherTreeNode*) tato_list_shift(stack);
		if (pivot->depth < depth || depth == -1) {
			TatoRelationsNode *it;
			TATO_RELATIONS_FOREACH(pivot->item->relations, it) {
				if (!filter || tato_relation_filter_eval(filter, it->relation, it->with)) {
					TatoFetcherTreeNode *child = tato_fetcher_tree_node_new(it->with, it->relation);
					if (tato_fetcher_tree_add(thiss, pivot, child)) {
						tato_list_push(stack, child);
					}
				}
			}
		}
	}
	tato_list_free(stack);
}


