#define _GNU_SOURCE

#include "db.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "data_parser.h"

TatoDb *tato_db_new() {
	TatoDb *thiss = malloc(sizeof(TatoDb));
	thiss->relations = tato_tree_int_new();
	thiss->items = tato_tree_int_new();
	thiss->langs = tato_tree_str_new();
	thiss->common = tato_tree_str_new();
	thiss->default_path = NULL;
	thiss->changed = false;
	return thiss;
}

void tato_db_free(TatoDb *thiss) {
	//free relations
	TatoTreeIntNode *it;
	TATO_TREE_INT_FOREACH(thiss->relations, it) {
		tato_relation_free(it->value);
	}
	tato_tree_int_free(thiss->relations);

	//free items
	TATO_TREE_INT_FOREACH(thiss->items, it) {
		tato_item_free(it->value);
	}
	tato_tree_int_free(thiss->items);

	//free langs
	TatoTreeStrNode *it2;
	TATO_TREE_STR_FOREACH(thiss->langs, it2) {
		tato_item_lang_free(it2->value);
	}
	tato_tree_str_free(thiss->langs);

	//free common strings
	TATO_TREE_STR_FOREACH(thiss->common, it2) {
		free((char *) it2->key);
	}
	tato_tree_str_free(thiss->common);

	//misc
	if (thiss->default_path) free(thiss->default_path);

	free(thiss);
}

void tato_db_changed_set(TatoDb *thiss, bool changed) {
	if (thiss->default_path != NULL) {
		free(thiss->default_path);
	}
	thiss->changed = changed;
}

bool tato_db_changed_get(TatoDb *thiss) {
	return thiss->changed;
}

void tato_db_default_path_set(TatoDb *thiss, char const *path) {

	thiss->default_path = strdup(path);
}

int tato_db_item_insert(TatoDb *thiss, TatoItem *item) {
	if (tato_item_lang_item_can_add(item->lang, item->str) >= 0)
		return DUPLICATE;

	if (!tato_tree_int_insert_unique(thiss->items, item->id, (void *) item))
		return ERROR;

	tato_item_lang_item_add(item->lang, item);
	return ADDED;
}

TatoItem *tato_db_item_new(
	TatoDb *thiss,
	TatoItemId id,
	char const *lang,
	char const *str,
	TatoItemFlags flags
) {
	TatoItem *item = tato_item_new(
		id,
		tato_db_lang_find_or_create(thiss, lang),
		str,
		flags
	);
	if (tato_db_item_insert(thiss, item) != ADDED) {
		tato_item_free(item);
		return NULL;
	}
	return item;
}

TatoItem *tato_db_item_add(
	TatoDb *thiss,
	char const *lang,
	char const *str,
	TatoItemFlags flags
) {
	return tato_db_item_new(
		thiss,
		tato_tree_int_max(thiss->items) + 1,
		lang,
		str,
		flags
	);
}

bool tato_db_item_delete(TatoDb *thiss, TatoItemId id) {
	TatoItem *item = tato_tree_int_remove(thiss->items, id);
	if (!item)
		return false;

	tato_item_lang_item_remove(item->lang, item);
	tato_item_delete(item);
	return true;
}

bool tato_db_item_merge_into(
	TatoDb *thiss,
	TatoItemId id,
	TatoItem *into
) {
	TatoItem *item = NULL;
	TatoRelationsNode *it = NULL;
	TatoRelationsNode *next = NULL;

	// merge an item into itself does nothing
	if (into->id == id) return false; 
	
	item = tato_tree_int_remove(thiss->items, id);
	if (!item)
		return false;

	tato_item_lang_item_remove(item->lang, item);

	// the problem come with "with" of relationNode
	// moreover here the problem that if we merge two direct translation,
	// we will delete a link which is not reported on the database

	for (it = item->relations; it != NULL; it = next) {
		int result = 0;
		int relationid = it->relation->id;
		next = it->next;
		result = tato_relation_link_change(it->relation, item, into);
		// if the link has become a self loof then we delete it from
		// the database
		if (result == LINK_CHANGE_SELF_LOOP) {
			tato_db_relation_delete(
				thiss,
				relationid
			);
		}

	}

	tato_item_delete(item);

	return true;
}

TatoItem *tato_db_item_find(TatoDb *thiss, TatoItemId id) {
	return (TatoItem *) tato_tree_int_find(thiss->items, id);
}

TatoItem *tato_db_item_rand(TatoDb *thiss) {
	return (TatoItem *) tato_tree_int_rand(thiss->items);
}

TatoItem *tato_db_item_rand_with_lang(TatoDb *thiss, char const *lang_name) {
	TatoItemLang *lang = tato_db_lang_find(thiss, lang_name);
	if (!lang)
		return NULL;

	return (TatoItem *) tato_tree_str_rand(lang->index);
}

void tato_db_items_search(
	TatoDb *thiss,
	TatoItemFetcher *fetcher,
	char const *str
) {
	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(thiss->langs, it) {
		tato_item_lang_items_search(
			it->value,
			fetcher,
			str
		);
	}
}

bool tato_db_relation_insert(
	TatoDb *thiss,
	TatoRelation *relation
) {
	return tato_tree_int_insert_unique(
		thiss->relations,
		relation->id,
		(void *) relation
	);
}

TatoRelation *tato_db_relation_new(
	TatoDb *thiss,
	TatoRelationId id,
	TatoItemId x,
	TatoItemId y,
	TatoRelationType type,
	TatoRelationFlags flags
) {
	TatoItem *item_x = tato_db_item_find(thiss, x);
	if (!item_x)
		return NULL;

	TatoItem *item_y = tato_db_item_find(thiss, y);
	if (!item_y)
		return NULL;

	TatoRelation *relation = tato_relation_new(
		id,
		item_x,
		item_y,
		type,
		flags
	);
	if (!tato_db_relation_insert(thiss, relation)) {
		tato_relation_free(relation);
		return NULL;
	}
	return relation;
}

TatoRelation *tato_db_relation_add(
	TatoDb *thiss,
	TatoItemId x,
	TatoItemId y,
	TatoRelationType type,
	TatoRelationFlags flags
) {
	return tato_db_relation_new(
		thiss,
		tato_tree_int_max(thiss->relations) + 1,
		x,
		y,
		type,
		flags
	);
}

TatoRelation *tato_db_relation_find(TatoDb *thiss, TatoRelationId id) {
	return (TatoRelation *) tato_tree_int_find(thiss->relations, id);
}

bool tato_db_relation_delete(TatoDb *thiss, TatoRelationId id) {
	TatoRelation *relation = tato_tree_int_remove(thiss->relations, id);
	if (!relation)
		return false;

	tato_relation_delete(relation);
	return true;
}

TatoItemLang *tato_db_lang_find(TatoDb *thiss, char const *code) {
	return (TatoItemLang *) tato_tree_str_find(thiss->langs, code);
}

TatoItemLang *tato_db_lang_find_or_create(TatoDb *thiss, char const *code) {
	TatoItemLang *found = tato_tree_str_find(thiss->langs, code);
	if (found) {
		return found;
	}
	TatoItemLang *lang = tato_item_lang_new(code);
	tato_tree_str_insert_unique(
		thiss->langs,
		lang->code,
		lang
	);
	return lang;
}

char const *tato_db_common_str(TatoDb *thiss, char const *str) {
	TatoTreeStrNode *node = tato_tree_str_node_find(thiss->common, str);
	if (node) {
		return node->key;
	}

	char const *newstr = strdup(str); 
	tato_tree_str_insert_unique(
		thiss->common,
		newstr,
		NULL
	);
	return newstr;
}

void tato_db_load(TatoDb *thiss, char const *path) {
	tato_db_default_path_set(thiss, path);
	TatoDataParser *parser = tato_data_parser_new(thiss);
	tato_data_parser_read(parser, path);
	tato_data_parser_free(parser);
}

void tato_db_fdump(TatoDb *thiss, FILE *f) {
	fprintf(f, "<tatodb>\n");
	TatoTreeIntNode *it = NULL;

	//dump items
	RB_FOREACH(it, TatoTreeInt_, thiss->items) {
		tato_item_dump(it->value, f);
	}

	//dump relations
	RB_FOREACH(it, TatoTreeInt_, thiss->relations) {
		tato_relation_dump(it->value, f);
	}
	
	fprintf(f, "</tatodb>\n");
}

bool tato_db_dump(TatoDb *thiss, char const *path) {
	char *tmp;
	if (asprintf(&tmp, "%s.tmp", path) == -1)
		return false;

	FILE *f = fopen(tmp, "w");
	if (!f)
		return false;

	tato_db_fdump(thiss, f);
	fclose(f);
	sync();	
	if (rename(tmp, path) != 0)
		return false;

	return true;
}

void tato_db_dump_forked(TatoDb *thiss, char const *path) {
	pid_t pid = fork();
	if (pid == 0) {
		tato_db_dump(thiss, path);
		_exit(0);
	}
}


/**
 * Write a dump of the database indexable by sphinx using
 * xml2pipe, the dump is written into the already opened file
 * f
 */

void tato_db_sphinx_fdump_lang(TatoItemLang *lang, FILE *f) {
	fprintf(
        f,
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<sphinx:docset>\n"
    );


	TatoTreeStrNode *it = NULL;

	//dump items
	TATO_TREE_STR_FOREACH(lang->index, it) {
		tato_item_sphinx_dump(it->value, f);
	}

	fprintf(f, "</sphinx:docset>\n");
}


/**
 * function to controll the dump process,
 * it will first open a tmp file, call the *_fdump function
 * to dump the database in it, and will then rename the file
 * this way if a error occurs, the previous file will not be
 * corrupted, only the .tmp (which is not supposed to be read)
 */
bool tato_db_sphinx_dump(TatoDb *thiss, char const *path) {

    char *tmp = NULL;
    char *final = NULL;
    FILE *f = NULL;
	TatoTreeStrNode *it = NULL;

	TATO_TREE_STR_FOREACH(thiss->langs, it) {
        if (asprintf(&final, "%s_%s", path, ((TatoItemLang*)it->value)->code) == -1)
            return false;
        if (asprintf(&tmp, "%s.tmp", final) == -1)
            return false;

        f = fopen(tmp, "w");
        if (!f)
            return false;

        tato_db_sphinx_fdump_lang(it->value, f);
        fclose(f);
        sync();	
        if (rename(tmp, final) != 0)
            return false;
        free(tmp);
        free(final);

	}

	return true;
}



/**
 * 
 */
void tato_db_sphinx_dump_forked(TatoDb *thiss, char const *path) {
	pid_t pid = fork();
	if (pid == 0) {
		tato_db_sphinx_dump(thiss, path);
		_exit(0);
	}
}



void tato_db_debug(TatoDb *thiss) {
	printf("items.size: %i\n", tato_tree_int_size(thiss->items));
	printf("relations.size: %i\n", tato_tree_int_size(thiss->relations));
	printf("langs.size: %i\n", tato_tree_str_size(thiss->langs));

	TatoTreeStrNode *it = NULL;
	TATO_TREE_STR_FOREACH(thiss->langs, it) {
		tato_item_lang_debug(it->value);
	}
}

