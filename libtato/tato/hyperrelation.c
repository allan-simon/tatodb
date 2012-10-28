#include "hyperrelation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "hyperrelations.h"
#include "xml.h"

TatoHyperRelation *tato_hyper_relation_new(
	TatoHyperRelationId id,
	TatoHyperItem *start,
	TatoHyperItem *end,
	TatoHyperRelationType type,
	TatoHyperRelationFlags flags
) {
	TatoHyperRelation *thiss = malloc(sizeof(TatoHyperRelation));
	thiss->id = id;
	thiss->start = start;

	thiss->ends = NULL;
	
	thiss->type = type;
	thiss->flags = flags;
	thiss->metas = NULL;

	TatoHyperItemsNode *node = tato_hyper_items_node_new(end);
	if (!tato_hyper_items_add(&(thiss->ends), node)) {
		tato_hyper_items_node_free(node);
	}


	tato_hyper_item_relation_add(start, thiss, end);
	return thiss;
}

void tato_hyper_relation_free(TatoHyperRelation *thiss) {
	tato_kvlist_clear(&thiss->metas);
	free(thiss);
    thiss = NULL;
}

void tato_hyper_relation_delete(TatoHyperRelation *thiss) {
	tato_hyper_relation_unlink_all(thiss);
	tato_hyper_relation_free(thiss);
}

void tato_hyper_relation_link(TatoHyperRelation *thiss) {

}


void tato_hyper_relation_link_add(TatoHyperRelation *thiss, TatoHyperItem *end) {
	TatoHyperItemsNode *node = tato_hyper_items_node_new(end);
	if (!tato_hyper_items_add(&(thiss->ends), node)) {
		tato_hyper_items_node_free(node);
		return;
	}

	tato_hyper_item_end_relation_add(
		thiss->start,
		thiss,
		end
	); //start => PREVIOUS + {end}
}


void tato_hyper_relation_unlink_all(TatoHyperRelation *thiss) {
	
	tato_hyper_item_start_relation_remove(thiss->start, thiss);

	TatoHyperItemsNode *it;
	TATO_HYPER_ITEMS_FOREACH(thiss->ends, it) {
		tato_hyper_item_end_relation_remove(it->item, thiss); 
	}
	tato_hyper_items_clear(&(thiss->ends));

}


/**
 *
 */
void tato_hyper_relation_unlink_one(TatoHyperRelation *thiss, TatoHyperItem *item) {
	if (thiss->ends->next == NULL) {
		tato_hyper_relation_delete(thiss);
	} else {
		tato_hyper_relations_delete(&(item->endofs), thiss);
		tato_hyper_items_delete(&(thiss->ends), item);
	}
}

/**
 *
 */
void tato_hyper_relation_link_change_end(
	TatoHyperRelation *thiss,
	TatoHyperItem *olditem,
	TatoHyperItem *newitem
) {
	// TODO can be done directly by replacing the pointer of the olditem by
	// the new one once we've found it.
	tato_hyper_items_delete(&(thiss->ends), olditem);
	TatoHyperItemsNode *newnode = tato_hyper_items_node_new(newitem);
	if (!tato_hyper_items_add(&(thiss->ends), newnode)) {
		tato_hyper_items_node_free(newnode);
	}
}

/**
 *
 */
void tato_hyper_relation_link_change_start(
	TatoHyperRelation *thiss,
	TatoHyperItem *olditem,
	TatoHyperItem *newitem
) {
	// TODO can be done directly by replacing the pointer of the olditem by
	// the new one once we've found it.
	if (thiss->start == olditem) {
		thiss->start = newitem;
	}
}





/**
 *
 */
void tato_hyper_relation_flags_set(TatoHyperRelation *thiss, TatoHyperRelationFlags const flags) {
	thiss->flags = flags;
}

void tato_hyper_relation_type_set(TatoHyperRelation *thiss, TatoHyperRelationType const type) {
	thiss->type = type;
}

bool tato_hyper_relation_meta_add(TatoHyperRelation *thiss, char const *key, char const *value) {
	return tato_kvlist_add(&thiss->metas, key, value);
}

char const *tato_hyper_relation_meta_get(TatoHyperRelation *thiss, char const *key) {
	return tato_kvlist_get(thiss->metas, key);
}

bool tato_hyper_relation_meta_set(TatoHyperRelation *thiss, char const *key, char const *value) {
	return tato_kvlist_set(thiss->metas, key, value);
}

bool tato_hyper_relation_meta_delete(TatoHyperRelation *thiss, char const *key) {
	return tato_kvlist_delete(&thiss->metas, key);
}

void tato_hyper_relation_dump(TatoHyperRelation *thiss, FILE *f) {
	fprintf(
		f,
		"<hyperrel id=\"%i\" start=\"%i\" end=\"%i\" type=\"%i\" flags=\"%i\">\n", 
		thiss->id,
		thiss->start->id,
		thiss->ends->item->id,
		thiss->type,
		thiss->flags
	);
	
	TatoHyperItemsNode *it;
	TATO_HYPER_ITEMS_FOREACH(thiss->ends->next, it) {
		fprintf(f, "<end id=\"%i\" />\n", 
		   it->item->id 
		);
	}

	if (thiss->metas != NULL) { 
		tato_kvlist_dump(thiss->metas, f, "meta");
	}
	fprintf(f, "</hyperrel>\n");
}

void tato_hyper_relation_debug(TatoHyperRelation *thiss) {
	printf("== HyperRelation ==\n");
	printf("id: %i\n", thiss->id);
	printf("start: %i\n", thiss->start->id);
 
	TatoHyperItemsNode *it;
	TATO_HYPER_ITEMS_FOREACH(thiss->ends, it) {
		printf("end :%i\n", it->item->id);
	}

	printf("type: %i\n", thiss->type);
	printf("flags: %i\n", thiss->flags);
}

//---- TatoHyperRelationFilter

TatoHyperRelationFilter *tato_hyper_relation_filter_new(
	TatoHyperRelationFilterCallback callback,
	void *data
) {
	TatoHyperRelationFilter *thiss = malloc(sizeof(TatoHyperRelationFilter));
	tato_hyper_relation_filter_init(thiss, callback, data);
	return thiss;
}

void tato_hyper_relation_filter_init(
	TatoHyperRelationFilter *thiss,
	TatoHyperRelationFilterCallback callback,
	void *data
) {
	thiss->callback = callback;
	thiss->data = data;
}

void tato_hyper_relation_filter_free(TatoHyperRelationFilter *thiss) {
	free(thiss);
}

bool tato_hyper_relation_filter_eval(
	TatoHyperRelationFilter *thiss,
	TatoHyperRelation *relation,
	TatoHyperItems *withs
) {
	return thiss->callback(thiss->data, relation, withs);
}

