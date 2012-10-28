#define _GNU_SOURCE

#include "hyperdb.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "hyper_data_parser.h"

TatoHyperDb *tato_hyper_db_new() {
	TatoHyperDb *thiss = malloc(sizeof(TatoHyperDb));
	thiss->relations = tato_tree_int_new();
	thiss->items = tato_tree_int_new();
	thiss->langs = tato_tree_str_new();
	thiss->common = tato_tree_str_new();
	thiss->default_path = NULL;
	thiss->changed = false;
	return thiss;
}

void tato_hyper_db_free(TatoHyperDb *thiss) {
	//free relations
	TatoTreeIntNode *it;
		TATO_TREE_INT_FOREACH(thiss->relations, it) {
			tato_hyper_relation_delete(it->value);
		}
		tato_tree_int_free(thiss->relations);

	//free items
	TATO_TREE_INT_FOREACH(thiss->items, it) {
		tato_hyper_item_free(it->value);
	}
	tato_tree_int_free(thiss->items);

	//free langs
	TatoTreeStrNode *it2;
	TATO_TREE_STR_FOREACH(thiss->langs, it2) {
		tato_hyper_item_lang_free(it2->value);
	}
	tato_tree_str_free(thiss->langs);

	//free common strings
	TATO_TREE_STR_FOREACH(thiss->common, it2) {
		free((char *) it2->key);
	}
	tato_tree_str_free(thiss->common);

	//misc
	if (thiss->default_path) free(thiss->default_path);

	if (thiss) free(thiss);
}

void tato_hyper_db_changed_set(TatoHyperDb *thiss, bool changed) {
	thiss->changed = changed;
}

bool tato_hyper_db_changed_get(TatoHyperDb *thiss) {
	return thiss->changed;
}

void tato_hyper_db_default_path_set(TatoHyperDb *thiss, char const *path) {
	if (thiss->default_path != NULL) {
		free(thiss->default_path);
	}
	thiss->default_path = strdup(path);
}

int tato_hyper_db_item_insert(TatoHyperDb *thiss, TatoHyperItem *item) {
	if (tato_hyper_item_lang_item_can_add(item->lang, item->str) >= 0)
		return ERROR;

	if (!tato_tree_int_insert_unique(thiss->items, item->id, (void *) item))
		return DUPLICATE;

	tato_hyper_item_lang_item_add(item->lang, item);
	return ADDED;
}

TatoHyperItem *tato_hyper_db_item_new(
	TatoHyperDb *thiss,
	TatoHyperItemId id,
	char const *lang,
	char const *str,
	TatoHyperItemFlags flags
) {
	TatoHyperItem *item = tato_hyper_item_new(
		id,
		tato_hyper_db_lang_find_or_create(thiss, lang),
		str,
		flags
	);
	if (tato_hyper_db_item_insert(thiss, item) != ADDED) {
		tato_hyper_item_free(item);
		return NULL;
	}
	return item;
}

TatoHyperItem *tato_hyper_db_item_add(
	TatoHyperDb *thiss,
	char const *lang,
	char const *str,
	TatoHyperItemFlags flags
) {
	return tato_hyper_db_item_new(
		thiss,
		tato_tree_int_max(thiss->items) + 1,
		lang,
		str,
		flags
	);
}

bool tato_hyper_db_item_delete(TatoHyperDb *thiss, TatoHyperItemId id) {
    
	TatoHyperItem *item = tato_tree_int_find(thiss->items, id);
	if (!item)
		return false;
	tato_hyper_item_lang_item_remove(item->lang, item);

	//Remove all the hyper relations for which "item" is the starting node
	TatoHyperRelationsNode *it, *next;
	for (it = item->startofs; it != NULL; it = next) {
		next = it->next;
		TatoHyperRelation *relation = it->relation;
		tato_hyper_db_relation_delete(thiss, relation->id); 
	}

	//Remove the item from all the hyperrelations for which "item" is the ending node
	for (it = item->endofs; it != NULL; it = next) {
		next = it->next;
		TatoHyperRelation *relation = it->relation;
		
		tato_hyper_db_relation_remove_end(thiss, relation->id, id); 
	}
	item->endofs = NULL;
	 tato_tree_int_remove(thiss->items, id);
	tato_hyper_item_delete(item);
	return true;
}

bool tato_hyper_db_item_merge_into(
	TatoHyperDb *thiss,
	TatoHyperItemId id,
	TatoHyperItem *into
) {
	// if we try to merge one item into itself
	if (into->id == id) return true;

	TatoHyperItem *item = tato_tree_int_remove(thiss->items, id);
	if (!item)
		return false;

	tato_hyper_item_lang_item_remove(item->lang, item);
	tato_hyper_item_merge_into(item, into);
	return true;
}

TatoHyperItem *tato_hyper_db_item_find(
	TatoHyperDb *thiss,
	TatoHyperItemId id
) {
	return (TatoHyperItem *) tato_tree_int_find(thiss->items, id);
}

TatoHyperItem *tato_hyper_db_item_rand(TatoHyperDb *thiss) {
	return (TatoHyperItem *) tato_tree_int_rand(thiss->items);
}

TatoHyperItem *tato_hyper_db_item_rand_with_lang(
	TatoHyperDb *thiss, char const *lang_name) {
	TatoHyperItemLang *lang = tato_hyper_db_lang_find(thiss, lang_name);
	if (!lang)
		return NULL;

	return (TatoHyperItem *) tato_tree_str_rand(lang->index);
}

void tato_hyper_db_items_search(
	TatoHyperDb *thiss,
	TatoHyperItemFetcher *fetcher,
	char const *str
) {
	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(thiss->langs, it) {
		tato_hyper_item_lang_items_search(
			it->value,
			fetcher,
			str
		);
	}
}


void tato_hyper_db_items_get_seq(
	TatoHyperDb *thiss,
	TatoHyperItemFetcher *fetcher
) {

	TatoTreeIntNode *it;
	fetcher->max_size = tato_tree_int_size(thiss->items);
	TATO_TREE_INT_FOREACH(thiss->items, it) {
		tato_hyper_item_fetcher_add(fetcher, it->value);
		if (tato_hyper_item_fetcher_eof(fetcher)) {
			return;
		}
	}
}





bool tato_hyper_db_relation_insert(
	TatoHyperDb *thiss,
	TatoHyperRelation *relation
) {
	return tato_tree_int_insert_unique(
		thiss->relations,
		relation->id,
		(void *) relation
	);
}

TatoHyperRelation *tato_hyper_db_relation_new(
	TatoHyperDb *thiss,
	TatoHyperRelationId id,
	TatoHyperItemId start,
	TatoHyperItemId end,
	TatoHyperRelationType type,
	TatoHyperRelationFlags flags
) {
	TatoHyperItem *item_start = tato_hyper_db_item_find(thiss, start);
	if (!item_start)
		return NULL;

	TatoHyperItem *item_end = tato_hyper_db_item_find(thiss, end);
	if (!item_end)
		return NULL;

	TatoHyperRelation *relation = tato_hyper_relation_new(
		id,
		item_start,
		item_end,
		type,
		flags
	);
	if (!tato_hyper_db_relation_insert(thiss, relation)) {
		tato_hyper_relation_free(relation);
		return NULL;
	}
	return relation;
}

TatoHyperRelation *tato_hyper_db_relation_add(
	TatoHyperDb *thiss,
	TatoHyperItemId start,
	TatoHyperItemId end,
	TatoHyperRelationType type,
	TatoHyperRelationFlags flags
) {
	return tato_hyper_db_relation_new(
		thiss,
		tato_tree_int_max(thiss->relations) + 1,
		start,
		end,
		type,
		flags
	);
}


TatoHyperRelation *tato_hyper_db_relation_add_end(
	TatoHyperDb *thiss,
	TatoHyperRelationId relation_id,
	TatoHyperItemId end
) {
	TatoHyperRelation* relation = tato_hyper_db_relation_find(thiss, relation_id);
	if (!relation)
		return NULL;
	TatoHyperItem *item_end = tato_hyper_db_item_find(thiss, end);
	if (!item_end)
		return NULL;
	tato_hyper_relation_link_add(relation, item_end);
	return relation;
}


TatoHyperRelation *tato_hyper_db_relation_remove_end(
	TatoHyperDb *thiss,
	TatoHyperRelationId relation_id,
	TatoHyperItemId end
) {
	TatoHyperRelation* relation = tato_hyper_db_relation_find(thiss, relation_id);
	TatoHyperItem *item_end = tato_hyper_db_item_find(thiss, end);
	if (item_end == NULL) {
		return NULL;
    }
	//tato_hyper_relation_unlink_one(relation, item_end);
	tato_hyper_relations_delete(&(item_end->endofs), relation);
	tato_hyper_items_delete(&(relation->ends), item_end);
	// if this item was last "end" of the hyper relation
	// then we remove also the hyper relation
	if (relation->ends == NULL) {
		tato_hyper_db_relation_delete(thiss, relation->id);
		relation = NULL;
	}
	return relation;
}






TatoHyperRelation *tato_hyper_db_relation_find(TatoHyperDb *thiss, TatoHyperRelationId id) {
	return (TatoHyperRelation *) tato_tree_int_find(thiss->relations, id);
}

bool tato_hyper_db_relation_delete(TatoHyperDb *thiss, TatoHyperRelationId id) {
	TatoHyperRelation *relation = tato_tree_int_remove(thiss->relations, id);
	if (!relation)
		return false;

	tato_hyper_relation_delete(relation);
	return true;
}

TatoHyperItemLang *tato_hyper_db_lang_find(TatoHyperDb *thiss, char const *code) {
	return (TatoHyperItemLang *) tato_tree_str_find(thiss->langs, code);
}

TatoHyperItemLang *tato_hyper_db_lang_find_or_create(TatoHyperDb *thiss, char const *code) {
	TatoHyperItemLang *found = tato_tree_str_find(thiss->langs, code);
	if (found) {
		return found;
	}
	TatoHyperItemLang *lang = tato_hyper_item_lang_new(code);
	tato_tree_str_insert_unique(
		thiss->langs,
		lang->code,
		lang
	);
	return lang;
}

char const *tato_hyper_db_common_str(TatoHyperDb *thiss, char const *str) {
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

void tato_hyper_db_load(TatoHyperDb *thiss, char const *path) {
	if (path != NULL) {
		printf("should load: %s\n", path);
		tato_hyper_db_default_path_set(thiss, path);
	}
	TatoHyperDataParser *parser = tato_hyper_data_parser_new(thiss);
	tato_hyper_data_parser_read(parser, path);
	tato_hyper_data_parser_free(parser);
}

void tato_hyper_db_fdump(TatoHyperDb *thiss, FILE *f) {
	fprintf(f, "<tatohyperdb>\n");
	TatoTreeIntNode *it = NULL;

	//dump items
	RB_FOREACH(it, TatoTreeInt_, thiss->items) {
		tato_hyper_item_dump(it->value, f);
	}

	//dump relations
	RB_FOREACH(it, TatoTreeInt_, thiss->relations) {
		tato_hyper_relation_dump(it->value, f);
	}
	
	fprintf(f, "</tatohyperdb>\n");
}

bool tato_hyper_db_dump(TatoHyperDb *thiss, char const *path) {

	const char * tempExtension = ".tmp";
	int size = strlen(path) + strlen(tempExtension) + 1;
	char *tmp = malloc(size);
	if (!tmp) {
		return false;
	}
	strcpy(tmp, path);
	strcat(tmp, tempExtension);

	FILE *f = fopen(tmp, "w");
	if (!f) {
		free(tmp);
		return false;
	}

	tato_hyper_db_fdump(thiss, f);
	fclose(f);
	sync();	
	if (rename(tmp, path) != 0) {
		free(tmp);
		return false;
	}

	free(tmp);
	return true;
}

bool tato_hyper_db_dump_forked(TatoHyperDb *thiss, char const *path) {
	pid_t pid = fork();
	if (pid == 0) {
		tato_hyper_db_dump(thiss, path);
		_exit(0);
	} else if (pid > 0) {
		return true;
	} else { 
		return false;
	}
}

void tato_hyper_db_debug(TatoHyperDb *thiss) {
	printf("items.size: %i\n", tato_tree_int_size(thiss->items));
	printf("relations.size: %i\n", tato_tree_int_size(thiss->relations));
	printf("langs.size: %i\n", tato_tree_str_size(thiss->langs));

	TatoTreeStrNode *it = NULL;
	TATO_TREE_STR_FOREACH(thiss->langs, it) {
		tato_hyper_item_lang_debug(it->value);
	}
}

