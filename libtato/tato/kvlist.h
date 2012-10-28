#ifndef _TATO_KVLIST_H
#define _TATO_KVLIST_H

#include <stdio.h>
#include <stdbool.h>

typedef struct TatoKvListNode_ {
	char const *key;
	char *value;
	struct TatoKvListNode_ *next;
} TatoKvListNode;

typedef TatoKvListNode *TatoKvList;

TatoKvListNode *tato_kvlist_node_new(char const *key, char const *value);
void tato_kvlist_node_free(TatoKvListNode *thiss);
void tato_kvlist_node_dump(TatoKvListNode *thiss, FILE *f, char const *name);

/**
* @brief Dump a key=>value into a file, in a sphinx indexable format 
*
* @param thiss Key=>value to dump
* @param f     File in which we will write the list dump
*/
void tato_kvlist_node_sphinx_dump(TatoKvListNode *thiss, FILE *f);

void tato_kvlist_clear(TatoKvList *thiss);
bool tato_kvlist_add(TatoKvList *thiss, char const *key, char const *value);
bool tato_kvlist_set(TatoKvList thiss, char const *key, char const *value);
bool tato_kvlist_delete(TatoKvList *thiss, char const *key);
char const *tato_kvlist_get(TatoKvList thiss, char const *key);
void tato_kvlist_dump(TatoKvList thiss, FILE *f, char const *name);
/**
* @brief Dump a list of keys=>values into a file, in a sphinx indexable XML
*
* @param thiss List of key=>value to dump
* @param f     File in which we will write the list dump
*/
void tato_kvlist_sphinx_dump(TatoKvList thiss, FILE *f);
void tato_kvlist_debug(TatoKvList thiss);

#define TATO_KVLIST_FOREACH(thiss, it) for(it = thiss; it != NULL; it = it->next)

#endif
