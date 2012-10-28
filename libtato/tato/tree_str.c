#include "tree_str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int tato_tree_str_node_cmp(TatoTreeStrNode const *ka, TatoTreeStrNode const *kb) {
	char const *a = ka->key, *b = kb->key;
	return strcmp(a, b);
}

RB_GENERATE(TatoTreeStr_, TatoTreeStrNode_, entry, tato_tree_str_node_cmp);

TatoTreeStr *tato_tree_str_new() {
	TatoTreeStr *thiss = malloc(sizeof(TatoTreeStr));
	RB_INIT(thiss);
	return thiss;
}

void tato_tree_str_clear(TatoTreeStr *thiss) {
	RB_CLEAR(TatoTreeStr_, thiss);
}

void tato_tree_str_free(TatoTreeStr *thiss) {
	tato_tree_str_clear(thiss);
	free(thiss);
}

void tato_tree_str_insert(TatoTreeStr *thiss, char const *key, void *value) {
	TatoTreeStrNode *node = malloc(sizeof(TatoTreeStrNode));
	node->key = key;
	node->value = value;
	RB_INSERT(TatoTreeStr_, thiss, node);
}

bool tato_tree_str_insert_unique(TatoTreeStr *thiss, char const *key, void *value) {
	TatoTreeStrNode *node = malloc(sizeof(TatoTreeStrNode));
	node->key = key;
	node->value = value;
	if (RB_INSERT_UNIQUE(TatoTreeStr_, thiss, node) != NULL) {
		free(node);
		return false;
	};
	return true;
}

void *tato_tree_str_remove_unique(TatoTreeStr *thiss, char const *key) {
	TatoTreeStrNode *node = tato_tree_str_node_find(thiss, key);
	if (!node)
		return NULL;

	RB_REMOVE(TatoTreeStr_, thiss, node);
	void *result = node->value;
	free(node);
	return result;
}

void *tato_tree_str_remove(TatoTreeStr *thiss, char const *key, void *value) {
	TatoTreeStrNode *node = tato_tree_str_node_find_multiple(thiss, key, value);
	if (!node)
		return NULL;

	RB_REMOVE(TatoTreeStr_, thiss, node);
	free(node);
	return value;
}

void *tato_tree_str_find(TatoTreeStr *thiss, char const *key) {
	TatoTreeStrNode *node = tato_tree_str_node_find(thiss, key);
	return (node) ? node->value : NULL;
}

void *tato_tree_str_rand(TatoTreeStr *thiss) {
	TatoTreeStrNode *node = RB_RAND(TatoTreeStr_, thiss);
	return (node) ? node->value : NULL;
}

char const *tato_tree_str_rand_key(TatoTreeStr *thiss) {
	TatoTreeStrNode *node = RB_RAND(TatoTreeStr_, thiss);
	return (node) ? node->key : 0;
}

char const *tato_tree_str_max(TatoTreeStr *thiss) {
	TatoTreeStrNode *node = RB_MAX(TatoTreeStr_, thiss);
	return node ? node->key : 0;
}

char const *tato_tree_str_min(TatoTreeStr *thiss) {
	TatoTreeStrNode *node = RB_MIN(TatoTreeStr_, thiss);
	return node ? node->key : 0;
}

unsigned int tato_tree_str_size(TatoTreeStr *thiss) {
	return RB_SIZE(thiss);
}

void tato_tree_str_debug(TatoTreeStr *thiss) {
	printf("=== TatoTreeStr (size: %i) ===\n", tato_tree_str_size(thiss));

	TatoTreeStrNode *it = NULL;
	TATO_TREE_STR_FOREACH(thiss, it) {
		printf("id: %s\n", it->key);
	}
}

void tato_tree_str_node_show(TatoTreeStrNode *node) {
	printf("<ul style='border-left: 1px solid gray;'>");
	if (!node)
		printf("nil");
	else {
		printf("<b style='color: %s;'>%s</b>", RB_COLOR(node, entry) == RB_RED ? "red" : "black", node->key);
		tato_tree_str_node_show(RB_LEFT(node, entry));
		tato_tree_str_node_show(RB_RIGHT(node, entry));
	}
	printf("</ul>");
}

void tato_tree_str_show(TatoTreeStr *thiss) {
	tato_tree_str_node_show(RB_ROOT(thiss));
}


TatoTreeStrNode *tato_tree_str_node_next(TatoTreeStrNode *node) {
	return RB_NEXT(TatoTreeStr_, node);
}

TatoTreeStrNode *tato_tree_str_node_next_similar(TatoTreeStrNode *node) {
	return RB_NEXT_SIMILAR(TatoTreeStr_, node);
}

TatoTreeStrNode *tato_tree_str_node_find_multiple(TatoTreeStr *thiss, char const *key, void *value) {
	TatoTreeStrNode *first = tato_tree_str_node_find(thiss, key);
	if (first->value == value) return first;

	TatoTreeStrNode *it = RB_NEXT_SIMILAR(TatoTreeStr_, first);
	while (it) {
		if (it->value == value) return it;
		it = RB_NEXT_SIMILAR(TatoTreeStr_, it);
	}

	it = RB_PREV_SIMILAR(TatoTreeStr_, first);
	while (it) {
		if (it->value == value) return it;
		it = RB_PREV_SIMILAR(TatoTreeStr_, it);
	}
	return NULL;
}

TatoTreeStrNode *tato_tree_str_node_find(TatoTreeStr *thiss, char const *key) {
	TatoTreeStrNode node_scanned;
	node_scanned.key = key;
	return RB_FIND(TatoTreeStr_, thiss, &node_scanned);
}

TatoTreeStrNode *tato_tree_str_node_find_first(TatoTreeStr *thiss, char const *key) {
	TatoTreeStrNode *node = tato_tree_str_node_find(thiss, key);
	return (node) ? RB_FIRST_SIMILAR(TatoTreeStr_, node) : NULL;
}

TatoTreeStrNode *tato_tree_str_node_nfind(TatoTreeStr *thiss, char const *key) {
	TatoTreeStrNode node_scanned;
	node_scanned.key = key;
	TatoTreeStrNode *node = RB_NFIND(TatoTreeStr_, thiss, &node_scanned);
	return (node) ? RB_FIRST_SIMILAR(TatoTreeStr_, node) : NULL;
}


