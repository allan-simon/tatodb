#include "tree_int.h"

#include <stdlib.h>
#include <stdio.h>

int tato_tree_int_node_cmp(TatoTreeIntNode const *ka, TatoTreeIntNode const *kb) {
	unsigned int a = ka->key, b = kb->key;
	if (a > b) 
		return 1;
	if (a < b) 
		return -1;
	return 0;
}

RB_GENERATE(TatoTreeInt_, TatoTreeIntNode_, entry, tato_tree_int_node_cmp);

TatoTreeInt *tato_tree_int_new() {
	TatoTreeInt *thiss = malloc(sizeof(TatoTreeInt));
	RB_INIT(thiss);
	return thiss;
}

void tato_tree_int_clear(TatoTreeInt *thiss) {
	RB_CLEAR(TatoTreeInt_, thiss);
}

void tato_tree_int_free(TatoTreeInt *thiss) {
	tato_tree_int_clear(thiss);
	free(thiss);
}

void tato_tree_int_insert(TatoTreeInt *thiss, unsigned int key, void *value) {
	TatoTreeIntNode *node = malloc(sizeof(TatoTreeIntNode));
	node->key = key;
	node->value = value;
	RB_INSERT(TatoTreeInt_, thiss, node);
}

bool tato_tree_int_insert_unique(TatoTreeInt *thiss, unsigned int key, void *value) {
	TatoTreeIntNode *node = malloc(sizeof(TatoTreeIntNode));
	node->key = key;
	node->value = value;
	if (RB_INSERT_UNIQUE(TatoTreeInt_, thiss, node) != NULL) {
		free(node);
		return false;
	};
	return true;
}

void *tato_tree_int_remove(TatoTreeInt *thiss, unsigned int key) {
	TatoTreeIntNode node_scanned;
	node_scanned.key = key;

	TatoTreeIntNode *node = RB_FIND(TatoTreeInt_, thiss, &node_scanned);
	if (!node)
		return NULL;

	RB_REMOVE(TatoTreeInt_, thiss, node);
	void *result = node->value;
	free(node);
	return result;
}

void *tato_tree_int_find(TatoTreeInt *thiss, unsigned int key) {
	TatoTreeIntNode node_scanned;
	node_scanned.key = key;

	TatoTreeIntNode *node = RB_FIND(TatoTreeInt_, thiss, &node_scanned);
	return (node) ? node->value : NULL;
}

void *tato_tree_int_rand(TatoTreeInt *thiss) {
	TatoTreeIntNode *node = RB_RAND(TatoTreeInt_, thiss);
	return (node) ? node->value : NULL;
}

unsigned int tato_tree_int_rand_key(TatoTreeInt *thiss) {
	TatoTreeIntNode *node = RB_RAND(TatoTreeInt_, thiss);
	return (node) ? node->key : 0;
}

unsigned int tato_tree_int_max(TatoTreeInt *thiss) {
	TatoTreeIntNode *node = RB_MAX(TatoTreeInt_, thiss);
	return node ? node->key : 0;
}

unsigned int tato_tree_int_min(TatoTreeInt *thiss) {
	TatoTreeIntNode *node = RB_MIN(TatoTreeInt_, thiss);
	return node ? node->key : 0;
}

unsigned int tato_tree_int_size(TatoTreeInt *thiss) {
	return RB_SIZE(thiss);
}

void tato_tree_int_debug(TatoTreeInt *thiss) {
	printf("=== TatoTreeInt (size: %i) ===\n", tato_tree_int_size(thiss));

	TatoTreeIntNode *it = NULL;
	TATO_TREE_INT_FOREACH(thiss, it) {
		printf("id: %i\n", it->key);
	}
}

void tato_tree_int_node_show(TatoTreeIntNode *node) {
	printf("<ul style='border-left: 1px solid gray;'>");
	if (!node)
		printf("nil");
	else {
		printf("<b style='color: %s;'>%i</b>", RB_COLOR(node, entry) == RB_RED ? "red" : "black", node->key);
		tato_tree_int_node_show(RB_LEFT(node, entry));
		tato_tree_int_node_show(RB_RIGHT(node, entry));
	}
	printf("</ul>");
}

void tato_tree_int_show(TatoTreeInt *thiss) {
	tato_tree_int_node_show(RB_ROOT(thiss));
}

