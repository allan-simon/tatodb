#ifndef _TATO_ITEM_H
#define _TATO_ITEM_H

#include <stdbool.h>
#include <stdio.h>

#include "tato.h"

#include "lang.h"
#include "kvlist.h"
#include "relations.h"

typedef unsigned int TatoItemId;
typedef unsigned int TatoItemFlags;

typedef struct TatoItem_ {
	TatoItemId id;
	TatoItemLang *lang;
	char *str;
	TatoItemFlags flags;
	TatoRelations relations;
	TatoKvList metas;
} TatoItem;

TatoItem *tato_item_new(TatoItemId id, TatoItemLang *lang, char const *str, TatoItemFlags flags);
void tato_item_free(TatoItem *thiss);
void tato_item_delete(TatoItem *thiss);
void tato_item_merge_into(TatoItem *thiss, TatoItem *into);

bool tato_item_str_set(TatoItem *thiss, char const *str);
void tato_item_flags_set(TatoItem *thiss, TatoItemFlags const flags);
char const *tato_item_lang_code(TatoItem *thiss);
bool tato_item_lang_set(TatoItem *thiss, TatoItemLang *lang);

bool tato_item_meta_add(TatoItem *thiss, char const *key, char const *value);
char const *tato_item_meta_get(TatoItem *thiss, char const *key);
bool tato_item_meta_set(TatoItem *thiss, char const *key, char const *value);
bool tato_item_meta_delete(TatoItem *thiss, char const *key);

void tato_item_relation_add(TatoItem *thiss, struct TatoRelation_ *relation, TatoItem *with);
bool tato_item_relation_remove(TatoItem *thiss, struct TatoRelation_ *relation);
void tato_item_unlink(TatoItem *thiss);
bool tato_item_related_with(TatoItem *thiss, TatoItemLang *lang);

int tato_item_str_cmp(TatoItem const *a, TatoItem const *b);

void tato_item_dump(TatoItem *thiss, FILE *f);

/**
* @brief Dump a TatoItem into a file, in a format indexable by sphinx 
*
* @param thiss Item to dump
* @param f     File in which the dump will be written
*/
void tato_item_sphinx_dump(TatoItem *thiss, FILE *f);
void tato_item_debug(TatoItem *thiss);

#endif
