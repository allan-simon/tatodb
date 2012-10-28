#ifndef _TATO_RELATION_H
#define _TATO_RELATION_H

#include <stdbool.h>

#include "tato.h"

#include "item.h"
#include "kvlist.h"

#define LINK_CHANGE_OK 0
#define LINK_CHANGE_NO_CHANGE 1
#define LINK_CHANGE_SELF_LOOP 2

typedef unsigned int TatoRelationId;
typedef unsigned int TatoRelationFlags;
typedef unsigned int TatoRelationType;

typedef struct TatoRelation_ {
	TatoRelationId id;
	TatoItem *x, *y;
	TatoRelationType type;
	TatoRelationFlags flags;
	TatoKvList metas;
} TatoRelation;

TatoRelation *tato_relation_new(TatoRelationId id, TatoItem *x, TatoItem *y, TatoRelationType type, TatoRelationFlags flags);
void tato_relation_free(TatoRelation *thiss);
void tato_relation_delete(TatoRelation *thiss);

void tato_relation_link(TatoRelation *thiss);
void tato_relation_unlink(TatoRelation *thiss);
int tato_relation_link_change(TatoRelation *thiss, TatoItem *olditem, TatoItem *newitem);

void tato_relation_flags_set(TatoRelation *thiss, TatoRelationFlags const flags);
void tato_relation_type_set(TatoRelation *thiss, TatoRelationType const type);

bool tato_relation_meta_add(TatoRelation *thiss, char const *key, char const *value);
char const *tato_relation_meta_get(TatoRelation *thiss, char const *key);
bool tato_relation_meta_set(TatoRelation *thiss, char const *key, char const *value);
bool tato_relation_meta_delete(TatoRelation *thiss, char const *key);

void tato_relation_debug(TatoRelation *thiss);
void tato_relation_dump(TatoRelation *thiss, FILE *f);


typedef bool (*TatoRelationFilterCallback)(void *, TatoRelation *, TatoItem *);

typedef struct TatoRelationFilter_ {
	TatoRelationFilterCallback callback;
	void *data;
} TatoRelationFilter;

TatoRelationFilter *tato_relation_filter_new(TatoRelationFilterCallback callback, void *data);
void tato_relation_filter_init(TatoRelationFilter *thiss, TatoRelationFilterCallback callback, void *data);
void tato_relation_filter_free(TatoRelationFilter *thiss);
bool tato_relation_filter_eval(TatoRelationFilter *thiss, TatoRelation *relation, TatoItem *with);


#endif
