#ifndef _TATO_TREE_STR_H
#define _TATO_TREE_STR_H

#include <stdbool.h>
#include "tree.h"

typedef struct TatoTreeStrNode_ {
	RB_ENTRY(TatoTreeStrNode_) entry;
	char const *key;
	void *value;
} TatoTreeStrNode;

typedef RB_HEAD(TatoTreeStr_, TatoTreeStrNode_) TatoTreeStr;
RB_PROTOTYPE(TatoTreeStr_, TatoTreeStrNode_, item, tato_tree_str_node_cmp);

TatoTreeStr *tato_tree_str_new();
void tato_tree_str_free(TatoTreeStr *thiss);
void tato_tree_str_clear(TatoTreeStr *thiss);
void tato_tree_str_insert(TatoTreeStr *thiss, char const *key, void *value);
bool tato_tree_str_insert_unique(TatoTreeStr *thiss, char const *key, void *value);
void *tato_tree_str_remove_unique(TatoTreeStr *thiss, char const *key);
void *tato_tree_str_remove(TatoTreeStr *thiss, char const *key, void *value);
void *tato_tree_str_find(TatoTreeStr *thiss, char const *key);
void *tato_tree_str_rand(TatoTreeStr *thiss);
char const *tato_tree_str_rand_key(TatoTreeStr *thiss);
char const *tato_tree_str_max(TatoTreeStr *thiss);
char const *tato_tree_str_min(TatoTreeStr *thiss);
unsigned int tato_tree_str_size(TatoTreeStr *thiss);
void tato_tree_str_debug(TatoTreeStr *thiss);
void tato_tree_str_show(TatoTreeStr *thiss);

TatoTreeStrNode *tato_tree_str_node_find(TatoTreeStr *thiss, char const *key);
TatoTreeStrNode *tato_tree_str_node_find_first(TatoTreeStr *thiss, char const *key);
TatoTreeStrNode *tato_tree_str_node_nfind(TatoTreeStr *thiss, char const *key);
TatoTreeStrNode *tato_tree_str_node_next(TatoTreeStrNode *node);
TatoTreeStrNode *tato_tree_str_node_next_similar(TatoTreeStrNode *node);
TatoTreeStrNode *tato_tree_str_node_find_multiple(TatoTreeStr *thiss, char const *key, void *value);
TatoTreeStrNode *tato_tree_str_node_first(TatoTreeStr *thiss, TatoTreeStrNode *node);

#define TATO_TREE_STR_FOREACH(thiss, it) RB_FOREACH(it, TatoTreeStr_, thiss)
#define TATO_TREE_STR_FOREACH_SIMILAR(thiss, it, key) for (it = tato_tree_str_node_find_first(thiss, key); it != NULL; it = tato_tree_str_node_next_similar(it))

#endif
