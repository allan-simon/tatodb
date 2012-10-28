#ifndef _TATO_DB_H
#define _TATO_DB_H

#include "tato.h"

#include "item.h"
#include "relation.h"
#include "lang.h"
#include "tree_int.h"
#include "tree_str.h"

#define ERROR 0
#define DUPLICATE 1
#define ADDED 2

typedef struct TatoDb_ {
	// index of all relations, indexed by their ids
	TatoTreeInt *relations;
	// index of all items, indexed by their ids
	TatoTreeInt *items;
	// index of by lang item indexes, indexed by their string
	TatoTreeStr *langs;
	TatoTreeStr *common;
	// path where to read/dump the xml representation of the database
	char *default_path;
	// if the database has changed since the last dump/read
	// to/from the database file
	bool changed;
} TatoDb;

TatoDb *tato_db_new();
void tato_db_free(TatoDb *thiss);
void tato_db_default_path_set(TatoDb *thiss, char const *path);
void tato_db_changed_set(TatoDb *thiss, bool changed);
bool tato_db_changed_get(TatoDb *thiss);

int tato_db_item_insert(TatoDb *thiss, TatoItem *item);
TatoItem *tato_db_item_new(
	TatoDb *thiss,
	TatoItemId id,
	char const *lang,
	char const *str,
	TatoItemFlags flags
);

TatoItem *tato_db_item_add(
	TatoDb *thiss,
	char const *lang,
	char const *str,
	TatoItemFlags flags
);
bool tato_db_item_delete(TatoDb *thiss, TatoItemId id);
TatoItem *tato_db_item_find(TatoDb *thiss, TatoItemId id);
TatoItem *tato_db_item_rand(TatoDb *thiss);
TatoItem *tato_db_item_rand_with_lang(TatoDb *thiss, char const *lang);
bool tato_db_item_merge_into(
	TatoDb *thiss,
	TatoItemId id,
	TatoItem *into
);

void tato_db_items_search(
	TatoDb *thiss,
	TatoItemFetcher *fetcher,
	char const *str
);

bool tato_db_relation_insert(
	TatoDb *thiss,
	TatoRelation *relation
);
TatoRelation *tato_db_relation_new(
	TatoDb *thiss,
	TatoRelationId id,
	TatoItemId x,
	TatoItemId y,
	TatoRelationType type,
	TatoRelationFlags flags
);
TatoRelation *tato_db_relation_add(
	TatoDb *thiss,
	TatoItemId x,
	TatoItemId y,
	TatoRelationType type,
	TatoRelationFlags flags
);
TatoRelation *tato_db_relation_find(TatoDb *thiss, TatoRelationId id);
bool tato_db_relation_delete(TatoDb *thiss, TatoRelationId id);

TatoItemLang *tato_db_lang_find(TatoDb *thiss, char const *code);
TatoItemLang *tato_db_lang_find_or_create(TatoDb *thiss, char const *code);

char const *tato_db_common_str(TatoDb *thiss, char const *str);

void tato_db_load(TatoDb *thiss, char const *path);
void tato_db_fdump(TatoDb *thiss, FILE *f);
bool tato_db_dump(TatoDb *thiss, char const *path);
void tato_db_dump_forked(TatoDb *thiss, char const *path);

//TODO maybe add a flag to permit people to compile it with or without
// sphinx support

/**
* @brief Function that dump the data into a XML indexable by Sphinx, in the
*        file opened by tato_db_sphinx_dump, dump only the items in the 
*        given language
*
* @param lang Language of which the items will be dumped
* @param f    File opened in which the data will be written
*/
void tato_db_sphinx_fdump_lang(TatoItemLang *lang, FILE *f);

/**
* @brief Function that controll file open part of the dump process, it
*        first dump in a temp file, and will rename it into the actual wanted
*        name, so that  if a previous dump exists, it will not be corrupted if
*        the current dump fails
*
* @param thiss Database structure to dump
* @param path  File in which the database will be dumped
*
* @return 
*/
bool tato_db_sphinx_dump(TatoDb *thiss, char const *path);

/**
* @brief Create a fork to launch the dump of the database in sphinx indexable
*        XML format, the fork is closed once the dump is finished
*
* @param thiss The database structure to dump
* @param path  Path of the file in which the data will be dumped
*/
void tato_db_sphinx_dump_forked(TatoDb *thiss, char const *path);



void tato_db_debug(TatoDb *thiss);

#endif
