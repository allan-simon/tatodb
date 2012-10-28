#ifndef _TATO_HYPER_ITEM_H
#define _TATO_HYPER_ITEM_H

#include <stdbool.h>
#include <stdio.h>

#include "tato.h"

#include "lang.h"
#include "kvlist.h"
#include "hyperrelations.h"

typedef unsigned int TatoHyperItemId;
typedef unsigned int TatoHyperItemFlags;

/**
 * a TatoHyperItem is a "node" of an hypergraph
 * it's 90% similar to a normal TatoHyperItem except, TatoHyperItems are inter connected
 * using hyperedge instead of standard edge
 *
 */

typedef struct TatoHyperItem_ {
	TatoHyperItemId id;
	TatoHyperItemLang *lang;
	char *str;
	TatoHyperItemFlags flags;
	TatoHyperRelations startofs;
	TatoHyperRelations endofs;
	TatoKvList metas;
} TatoHyperItem;

TatoHyperItem *tato_hyper_item_new(
	TatoHyperItemId id,
	TatoHyperItemLang *lang,
	char const *str,
	TatoHyperItemFlags flags
);

void tato_hyper_item_free(TatoHyperItem *thiss);
void tato_hyper_item_delete(TatoHyperItem *thiss);
void tato_hyper_item_merge_into(TatoHyperItem *thiss, TatoHyperItem *into);

bool tato_hyper_item_str_set(TatoHyperItem *thiss, char const *str);
void tato_hyper_item_flags_set(TatoHyperItem *thiss, TatoHyperItemFlags const flags);
char const *tato_hyper_item_lang_code(TatoHyperItem *thiss);
bool tato_hyper_item_lang_set(TatoHyperItem *thiss, TatoHyperItemLang *lang);

bool tato_hyper_item_meta_add(TatoHyperItem *thiss, char const *key, char const *value);
char const *tato_hyper_item_meta_get(TatoHyperItem *thiss, char const *key);
bool tato_hyper_item_meta_set(TatoHyperItem *thiss, char const *key, char const *value);
bool tato_hyper_item_meta_delete(TatoHyperItem *thiss, char const *key);

/**
 * add the given hyper relation to the list of those starting
 * from the given hyper item
 */
void tato_hyper_item_relation_add(
	TatoHyperItem *thiss,
	struct TatoHyperRelation_ *relation,
	TatoHyperItem *with
);

void tato_hyper_item_end_relation_add(
	TatoHyperItem *thiss,
	struct TatoHyperRelation_ *relation,
	TatoHyperItem *with
);



bool tato_hyper_item_start_relation_remove(TatoHyperItem *thiss, struct TatoHyperRelation_ *relation);
bool tato_hyper_item_end_relation_remove(TatoHyperItem *thiss, struct TatoHyperRelation_ *relation);
void tato_hyper_item_unlink(TatoHyperItem *thiss);
bool tato_hyper_item_related_with(TatoHyperItem *thiss, TatoHyperItemLang *lang);

int tato_hyper_item_str_cmp(TatoHyperItem const *a, TatoHyperItem const *b);
void tato_hyper_item_dump(TatoHyperItem *thiss, FILE *f);
void tato_hyper_item_debug(TatoHyperItem *thiss);

#endif
