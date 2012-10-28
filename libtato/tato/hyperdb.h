#ifndef _TATO_HYPER_DB_H
#define _TATO_HYPER_DB_H

#include "tato.h"

#include "hyperitem.h"
#include "hyperrelation.h"
#include "lang.h"
#include "tree_int.h"
#include "tree_str.h"

#define ERROR 0
#define DUPLICATE 1
#define ADDED 2

typedef struct TatoHyperDb_ {
	TatoTreeInt *relations;
	TatoTreeInt *items;
	TatoTreeStr *langs;
	TatoTreeStr *common;
	char *default_path;
	bool changed;
} TatoHyperDb;


/***********************
 *  hyper db related   *
 ***********************/

/**
 * Instanciate a new hyperdb
 * return a pointer on the newly created hyperdb
 */
TatoHyperDb *tato_hyper_db_new();
/**
 *
 */
void tato_hyper_db_free(TatoHyperDb *thiss);
void tato_hyper_db_default_path_set(TatoHyperDb *thiss, char const *path);
void tato_hyper_db_changed_set(TatoHyperDb *thiss, bool changed);
bool tato_hyper_db_changed_get(TatoHyperDb *thiss);

/**
 * called by tato_hyper_db_item_add
 * will add the newly create item to the current database
 * and will update accorindgly all the indexes
 */
int tato_hyper_db_item_insert(TatoHyperDb *thiss, TatoHyperItem *item);
TatoHyperItem *tato_hyper_db_item_new(
	TatoHyperDb *thiss,
	TatoHyperItemId id,
	char const *lang,
	char const *str,
	TatoHyperItemFlags flags
);

/**
 * Create a new hyper item in the given database
 * and will add it to all the appropriate indexes
 */
TatoHyperItem *tato_hyper_db_item_add(
	TatoHyperDb *thiss,
	char const *lang,
	char const *str,
	TatoHyperItemFlags flags
);

/**
 * Delete the hyper item identify by the given id on the given database
 * and free it
 * and will remove it from all the indexes referencing it
 * it will also remove all the hyper relations for which the item is the starting
 * item and also the relation for which the item is the only ending item, and remove
 * those hyper relations from the indexes and free them
 */
bool tato_hyper_db_item_delete(TatoHyperDb *thiss, TatoHyperItemId id);
TatoHyperItem *tato_hyper_db_item_find(TatoHyperDb *thiss, TatoHyperItemId id);
TatoHyperItem *tato_hyper_db_item_rand(TatoHyperDb *thiss);
TatoHyperItem *tato_hyper_db_item_rand_with_lang(TatoHyperDb *thiss, char const *lang);
bool tato_hyper_db_item_merge_into(
	TatoHyperDb *thiss,
	TatoHyperItemId id,
	TatoHyperItem *into
);

/**
 * Return all the items in "thiss" which are EXACTLY matching "str"
 * and will put them in 'fetcher'
 * fetcher decide the offset and the limit of result that will be returned
 */
void tato_hyper_db_items_search(
	TatoHyperDb *thiss,
	TatoHyperItemFetcher *fetcher,
	char const *str
);

/**
 * Return the "offset"th hyperitems and the "size" following 
 */
void tato_hyper_db_items_get_seq(
	TatoHyperDb *thiss,
	TatoHyperItemFetcher *fetcher
);

bool tato_hyper_db_relation_insert(
	TatoHyperDb *thiss,
	TatoHyperRelation *relation
);

/**
 * called by relation_add
 */
TatoHyperRelation *tato_hyper_db_relation_new(
	TatoHyperDb *thiss,
	TatoHyperRelationId id,
	TatoHyperItemId start,
	TatoHyperItemId end,
	TatoHyperRelationType type,
	TatoHyperRelationFlags flags
);
TatoHyperRelation *tato_hyper_db_relation_add(
	TatoHyperDb *thiss,
	TatoHyperItemId start,
	TatoHyperItemId end,
	TatoHyperRelationType type,
	TatoHyperRelationFlags flags
);


/**
 * add an item to list of the "end" items of the given hyper relation
 */
TatoHyperRelation *tato_hyper_db_relation_add_end(
	TatoHyperDb *thiss,
	TatoHyperRelationId relation_id,
	TatoHyperItemId end
);


/**
 * remove an item to list of the "end" items of the given hyper relation
 */
TatoHyperRelation *tato_hyper_db_relation_remove_end(
	TatoHyperDb *thiss,
	TatoHyperRelationId relation_id,
	TatoHyperItemId end
);



TatoHyperRelation *tato_hyper_db_relation_find(TatoHyperDb *thiss, TatoHyperRelationId id);
bool tato_hyper_db_relation_delete(TatoHyperDb *thiss, TatoHyperRelationId id);

TatoHyperItemLang *tato_hyper_db_lang_find(TatoHyperDb *thiss, char const *code);
TatoHyperItemLang *tato_hyper_db_lang_find_or_create(TatoHyperDb *thiss, char const *code);

char const *tato_hyper_db_common_str(TatoHyperDb *thiss, char const *str);

void tato_hyper_db_load(TatoHyperDb *thiss, char const *path);
void tato_hyper_db_fdump(TatoHyperDb *thiss, FILE *f);
bool tato_hyper_db_dump(TatoHyperDb *thiss, char const *path);
bool tato_hyper_db_dump_forked(TatoHyperDb *thiss, char const *path);

void tato_hyper_db_debug(TatoHyperDb *thiss);

#endif

