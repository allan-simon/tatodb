#include "hyperitem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hyperrelations.h"
#include "hyperrelation.h"
#include "xml.h"

TatoHyperItem *tato_hyper_item_new(
	TatoHyperItemId id,
	TatoHyperItemLang *lang,
	char const *str,
	TatoHyperItemFlags flags
) {
	TatoHyperItem *thiss = malloc(sizeof(TatoHyperItem));
	thiss->id = id;
	thiss->str = strdup(str);
	thiss->flags = flags;
	thiss->lang = lang;
	thiss->startofs = NULL;
	thiss->endofs = NULL;
	thiss->metas = NULL;
	return thiss;
}

void tato_hyper_item_free(TatoHyperItem *thiss) {
	if (thiss->metas) tato_kvlist_clear(&thiss->metas);
	if (thiss->startofs) tato_hyper_relations_clear(&thiss->startofs);
	if (thiss->endofs) tato_hyper_relations_clear(&thiss->startofs);
	if (thiss->str) free(thiss->str);
	free(thiss);	
    thiss = NULL;
}

void tato_hyper_item_delete(TatoHyperItem *thiss) {
	tato_hyper_item_unlink(thiss);
	tato_hyper_item_free(thiss);
}

void tato_hyper_item_merge_into(TatoHyperItem *thiss, TatoHyperItem *into) {
	TatoHyperRelationsNode *it, *next;
	// merging an item into itself do nothing
	if (thiss == into) return;

	// we update the relation starting from the item to merge
	for (it = thiss->startofs; it != NULL; it = next) {
		next = it->next;
		// we update the link itself
		tato_hyper_relation_link_change_start(it->relation, thiss, into);

		// we add the reference to the item that will kept
		TatoHyperRelationsNode *start_node = tato_hyper_relations_node_new(it->relation);
		if (!tato_hyper_relations_add(&(into->startofs), start_node)) {
			tato_hyper_relations_node_free(start_node);
		}
		
		tato_hyper_relations_delete(&(thiss->startofs), it->relation); 
	}
	// the startofs list has been freed, we reset its pointer to avoid a
	// 2nd free
	thiss->startofs = NULL;


	// we update the relation ending to the item to merge
	for (it = thiss->endofs; it != NULL; it = next) {
		next = it->next;

		// we update the link itself
		tato_hyper_relation_link_change_end(it->relation, thiss, into);

		// we add the reference to the item that will kept
		TatoHyperRelationsNode *end_node = tato_hyper_relations_node_new(it->relation);
		if (!tato_hyper_relations_add(&(into->endofs), end_node)) {
			tato_hyper_relations_node_free(end_node);
		}
		
		tato_hyper_relations_delete(&(thiss->endofs), it->relation); 
	}
	// the endofs list has been freed, we reset its pointer to avoid a
	// 2nd free
	thiss->endofs = NULL;

	tato_hyper_item_free(thiss);
	//tato_hyper_item_delete(thiss);
}

bool tato_hyper_item_str_set(TatoHyperItem *thiss, char const *str) {
	if (tato_hyper_item_lang_item_can_add(thiss->lang, str) >= 0)
		return false;

	tato_hyper_item_lang_item_remove(thiss->lang, thiss);
	free(thiss->str);

	thiss->str = strdup(str);
	tato_hyper_item_lang_item_add(thiss->lang, thiss);
	return true;
}

char const *tato_hyper_item_lang_code(TatoHyperItem *thiss) {
	return thiss->lang->code;
}

bool tato_hyper_item_lang_set(TatoHyperItem *thiss, TatoHyperItemLang *lang) {
	if (tato_hyper_item_lang_item_can_add(lang, thiss->str) >= 0)
		return false;

	tato_hyper_item_lang_item_remove(thiss->lang, thiss);

	thiss->lang = lang;
	tato_hyper_item_lang_item_add(lang, thiss);
	return true;
}

void tato_hyper_item_flags_set(TatoHyperItem *thiss, TatoHyperItemFlags const flags) {
	thiss->flags = flags;
}

bool tato_hyper_item_meta_add(TatoHyperItem *thiss, char const *key, char const *value) {
	return tato_kvlist_add(&thiss->metas, key, value);
}

char const *tato_hyper_item_meta_get(TatoHyperItem *thiss, char const *key) {
	return tato_kvlist_get(thiss->metas, key);
}

bool tato_hyper_item_meta_set(TatoHyperItem *thiss, char const *key, char const *value) {
	return tato_kvlist_set(thiss->metas, key, value);
}

bool tato_hyper_item_meta_delete(TatoHyperItem *thiss, char const *key) {
	return tato_kvlist_delete(&thiss->metas, key);
}

void tato_hyper_item_relation_add(
	TatoHyperItem *thiss,
	TatoHyperRelation *relation,
	TatoHyperItem *with
) {
	TatoHyperRelationsNode *start_node = tato_hyper_relations_node_new(relation);
	if (!tato_hyper_relations_add(&(thiss->startofs), start_node)) {
		tato_hyper_relations_node_free(start_node);
	}
	TatoHyperRelationsNode *end_node = tato_hyper_relations_node_new(relation);
	if (!tato_hyper_relations_add(&(with->endofs), end_node)) {
		tato_hyper_relations_node_free(end_node);
	}
}


void tato_hyper_item_end_relation_add(
	TatoHyperItem *thiss,
	TatoHyperRelation *relation,
	TatoHyperItem *with
) {
	thiss;
	TatoHyperRelationsNode *end_node = tato_hyper_relations_node_new(relation);
	if (!tato_hyper_relations_add(&(with->endofs), end_node)) {
		tato_hyper_relations_node_free(end_node);
	}
}




bool tato_hyper_item_start_relation_remove(TatoHyperItem *thiss, TatoHyperRelation *relation) {
	return tato_hyper_relations_delete(&(thiss->startofs), relation);
}

bool tato_hyper_item_end_relation_remove(TatoHyperItem *thiss, TatoHyperRelation *relation) {
	return tato_hyper_relations_delete(&(thiss->endofs), relation);
}




void tato_hyper_item_unlink(TatoHyperItem *thiss) {

	//TATO_HYPER_RELATIONS_FOREACH(thiss->startofs, it) {
	TatoHyperRelationsNode *it = thiss->startofs;
	TatoHyperRelationsNode *next;

	for (it = thiss->startofs; it != NULL; it = next) {
		next = it->next;
		tato_hyper_relation_delete(it->relation);
	}

	for (it = thiss->endofs; it != NULL; it = next) {
		next = it->next;
		tato_hyper_relation_unlink_one(it->relation, thiss);
	}
}

bool tato_hyper_item_related_with(TatoHyperItem *thiss, TatoHyperItemLang *lang) {
	return (lang == NULL)
		? (thiss->startofs == NULL)
		: tato_hyper_relations_related_with(thiss->startofs, lang);	
}

void tato_hyper_item_dump(TatoHyperItem *thiss, FILE *f) {
	bool opened = (thiss->metas != NULL);
	char *str = (char *) tato_xml_escape(thiss->str);
	fprintf(f, "<hyperitem id=\"%i\" lang=\"%s\" str=\"%s\" flags=\"%i\"%c>\n",
		thiss->id,
		tato_hyper_item_lang_code(thiss),
		str,
		thiss->flags,
		opened ? ' ' : '/'
	);
	free(str);
	if (opened) {
		tato_kvlist_dump(thiss->metas, f, "meta");
		fprintf(f, "</hyperitem>\n"); 
	}
}

void tato_hyper_item_debug(TatoHyperItem *thiss) {
	printf("== TatoHyperItem ==\n");
	printf("id: %i\n", thiss->id);
	printf("str: %s\n", thiss->str);
	printf("lang: %s\n", tato_hyper_item_lang_code(thiss));
	tato_kvlist_debug(thiss->metas);	
	tato_hyper_relations_debug(thiss->startofs);	
}

