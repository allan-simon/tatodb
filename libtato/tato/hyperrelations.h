#ifndef _TATO_HYPER_RELATIONS_H
#define _TATO_HYPER_RELATIONS_H

#include <stdbool.h>

#include "tato.h"

/**
 * Linked list for hyper relations
 *
 */
typedef struct TatoHyperRelationsNode_ {
	struct TatoHyperRelation_ *relation;
	struct TatoHyperRelationsNode_ * next;
} TatoHyperRelationsNode;

typedef TatoHyperRelationsNode *TatoHyperRelations;

/**
 * Create a new linked-list of TatoHyperRelations with the one in argument as the 
 * first one
 * return a pointer on the newly created linked-list 
 */
TatoHyperRelationsNode *tato_hyper_relations_node_new(struct TatoHyperRelation_ *relation);

/**
 * TODO: write doc
 */
void tato_hyper_relations_node_free(TatoHyperRelationsNode *thiss);

/**
 * TODO: write doc
 */
void tato_hyper_relations_clear(TatoHyperRelations *thiss);

/**
 * Remove an hyper relation from the linked list
 * return true if it could have been removed, false otherwise
 */
bool tato_hyper_relations_delete(
	TatoHyperRelations *thiss,
	struct TatoHyperRelation_ *relation
);

/**
 * Add a new hyper relation node to the linked list
 * duplicate are not added
 */
bool tato_hyper_relations_add(TatoHyperRelations *thiss, TatoHyperRelationsNode *node);

/**
 * check if the given hyper relation is linked to at least one
 * item of the given language
 */
bool tato_hyper_relations_related_with(
	TatoHyperRelations thiss,
	struct TatoHyperItemLang_ *lang
);

/**
 * Print the internal structure of the given linked-list of HyperRelations
 * on the standard output
 */
void tato_hyper_relations_debug(TatoHyperRelations thiss);

#define TATO_HYPER_RELATIONS_FOREACH(thiss, it) \
	for (it = thiss; it != NULL; it = it->next)

#endif
