#ifndef _TATO_RELATIONS_H
#define _TATO_RELATIONS_H

#include <stdbool.h>

#include "tato.h"

/**
 * Linked-list for relations
 * used by TatoItem to have access to all its relations.
 */


typedef struct TatoRelationsNode_ {
	struct TatoRelation_ *relation;
	struct TatoItem_ *with;
	struct TatoRelationsNode_ * next;
} TatoRelationsNode;

TatoRelationsNode *tato_relations_node_new(struct TatoRelation_ *relation, struct TatoItem_ *with);
void tato_relations_node_free(TatoRelationsNode *thiss);

typedef TatoRelationsNode *TatoRelations;

void tato_relations_clear(TatoRelations *thiss);
bool tato_relations_delete(TatoRelations *thiss, struct TatoRelation_ *relation);
bool tato_relations_add(TatoRelations *thiss, TatoRelationsNode *node);
bool tato_relations_related_with(TatoRelations thiss, struct TatoItemLang_ *lang);
void tato_relations_debug(TatoRelations thiss);
void tato_relations_unlink(TatoRelations *thiss);

#define TATO_RELATIONS_FOREACH(thiss, it) for (it = thiss; it != NULL; it = it->next)

#endif
