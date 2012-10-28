#ifndef _TATO_TREE_INT_H
#define _TATO_TREE_INT_H

#include <stdbool.h>

#include "tree.h"

typedef struct TatoTreeIntNode_ {
	RB_ENTRY(TatoTreeIntNode_) entry;
	unsigned int key;
	void *value;
} TatoTreeIntNode;

typedef RB_HEAD(TatoTreeInt_, TatoTreeIntNode_) TatoTreeInt;
RB_PROTOTYPE(TatoTreeInt_, TatoTreeIntNode_, item, tato_tree_int_node_cmp);

TatoTreeInt *tato_tree_int_new();
void tato_tree_int_clear(TatoTreeInt *thiss);
void tato_tree_int_free(TatoTreeInt *thiss);
void tato_tree_int_insert(TatoTreeInt *thiss, unsigned int key, void *value);
bool tato_tree_int_insert_unique(TatoTreeInt *thiss, unsigned int key, void *value);
void *tato_tree_int_remove(TatoTreeInt *thiss, unsigned int key);
void *tato_tree_int_find(TatoTreeInt *thiss, unsigned int key);
void *tato_tree_int_rand(TatoTreeInt *thiss);
unsigned int tato_tree_int_rand_key(TatoTreeInt *thiss);
unsigned int tato_tree_int_max(TatoTreeInt *thiss);
unsigned int tato_tree_int_min(TatoTreeInt *thiss);
unsigned int tato_tree_int_size(TatoTreeInt *thiss);
void tato_tree_int_debug(TatoTreeInt *thiss);
void tato_tree_int_show(TatoTreeInt *thiss);

#define TATO_TREE_INT_FOREACH(thiss, it) RB_FOREACH(it, TatoTreeInt_, thiss)

#endif
