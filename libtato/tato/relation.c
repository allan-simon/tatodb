#include "relation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "relations.h"
#include "xml.h"

TatoRelation *tato_relation_new(TatoRelationId id, TatoItem *x, TatoItem *y, TatoRelationType type, TatoRelationFlags flags) {
	TatoRelation *thiss = malloc(sizeof(TatoRelation));
	thiss->id = id;
	thiss->x = x;
	thiss->y = y;
	thiss->type = type;
	thiss->flags = flags;
	thiss->metas = NULL;
	tato_relation_link(thiss);
	return thiss;
}

void tato_relation_free(TatoRelation *thiss) {
	tato_kvlist_clear(&thiss->metas);
	free(thiss);
}

void tato_relation_delete(TatoRelation *thiss) {
	tato_relation_unlink(thiss);
	tato_relation_free(thiss);
}

void tato_relation_link(TatoRelation *thiss) {
	tato_item_relation_add(thiss->x, thiss, thiss->y); //x => y
	tato_item_relation_add(thiss->y, thiss, thiss->x); //y => x
}

void tato_relation_unlink(TatoRelation *thiss) {
	tato_item_relation_remove(thiss->x, thiss);
	thiss->x = NULL;

	tato_item_relation_remove(thiss->y, thiss);
	thiss->y = NULL;
}

int tato_relation_link_change(TatoRelation *thiss, TatoItem *olditem, TatoItem *newitem) {


	if (thiss->x == olditem) {
		tato_item_relation_remove(olditem, thiss);
		tato_item_relation_remove(thiss->y, thiss);
		// if we're going to replace 
		// X <----> Y
		// by X <----> X then we only remove the link
		// in order to not create self link
		if (thiss->y == newitem) {
			
			return LINK_CHANGE_SELF_LOOP;
		}
		thiss->x = newitem;

		tato_item_relation_add(newitem, thiss, thiss->y);
		tato_item_relation_add(thiss->y, thiss, newitem);

		return LINK_CHANGE_OK;
	}
	if (thiss->y == olditem) {
		tato_item_relation_remove(olditem, thiss);
		tato_item_relation_remove(thiss->x, thiss);

		if (thiss->x == newitem) {
			return LINK_CHANGE_SELF_LOOP;
		}

		thiss->y = newitem;
		tato_item_relation_add(newitem, thiss, thiss->x);
		tato_item_relation_add(thiss->x, thiss, newitem);

		return LINK_CHANGE_OK;
	}
	return LINK_CHANGE_NO_CHANGE;
}

void tato_relation_flags_set(TatoRelation *thiss, TatoRelationFlags const flags) {
	thiss->flags = flags;
}

void tato_relation_type_set(TatoRelation *thiss, TatoRelationType const type) {
	thiss->type = type;
}

bool tato_relation_meta_add(TatoRelation *thiss, char const *key, char const *value) {
	return tato_kvlist_add(&thiss->metas, key, value);
}

char const *tato_relation_meta_get(TatoRelation *thiss, char const *key) {
	return tato_kvlist_get(thiss->metas, key);
}

bool tato_relation_meta_set(TatoRelation *thiss, char const *key, char const *value) {
	return tato_kvlist_set(thiss->metas, key, value);
}

bool tato_relation_meta_delete(TatoRelation *thiss, char const *key) {
	return tato_kvlist_delete(&thiss->metas, key);
}

void tato_relation_dump(TatoRelation *thiss, FILE *f) {
	bool opened = (thiss->metas != NULL);
	fprintf(f, "<rel id=\"%i\" x=\"%i\" y=\"%i\" type=\"%i\" flags=\"%i\"%c>\n", 
		thiss->id,
		thiss->x->id,
		thiss->y->id,
		thiss->type,
		thiss->flags,
		opened ? ' ' : '/'
	);
	if (opened) {
		tato_kvlist_dump(thiss->metas, f, "meta");
		fprintf(f, "</rel>\n");
	}
}

void tato_relation_debug(TatoRelation *thiss) {
	printf("== Relation ==\n");
	printf("id: %i\n", thiss->id);
	printf("x: %i\n", thiss->x->id);
	printf("y: %i\n", thiss->y->id);
	printf("type: %i\n", thiss->type);
	printf("flags: %i\n", thiss->flags);
}

//---- TatoRelationFilter

TatoRelationFilter *tato_relation_filter_new(TatoRelationFilterCallback callback, void *data) {
	TatoRelationFilter *thiss = malloc(sizeof(TatoRelationFilter));
	tato_relation_filter_init(thiss, callback, data);
	return thiss;
}

void tato_relation_filter_init(TatoRelationFilter *thiss, TatoRelationFilterCallback callback, void *data) {
	thiss->callback = callback;
	thiss->data = data;
}

void tato_relation_filter_free(TatoRelationFilter *thiss) {
	free(thiss);
}

bool tato_relation_filter_eval(TatoRelationFilter *thiss, TatoRelation *relation, TatoItem *with) {
	return thiss->callback(thiss->data, relation, with);
}

