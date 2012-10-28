#include "hyperrelations.h"

#include <stdlib.h>
#include <stdio.h>

#include "hyperrelation.h"
#include "lang.h"

/**
 * TODO the code is 90% the same than the one in relations.c
 */

TatoHyperRelationsNode *tato_hyper_relations_node_new(TatoHyperRelation *relation) {
	TatoHyperRelationsNode *thiss = malloc(sizeof(TatoHyperRelationsNode));
	thiss->relation = relation;
	thiss->next = NULL;
	return thiss;
}

void tato_hyper_relations_node_free(TatoHyperRelationsNode *thiss) {
	free(thiss);
}

void tato_hyper_relations_clear(TatoHyperRelations *thiss) {
	TatoHyperRelationsNode *it, *next;

	for (it = *thiss; it != NULL; it = next) {
		next = it->next;
		tato_hyper_relations_node_free(it);
	}
	*thiss = NULL;
}

bool tato_hyper_relations_add(
	TatoHyperRelations *thiss,
	TatoHyperRelationsNode *node
) {
	if (*thiss == NULL) {
		*thiss = node;
		return true;
	}
	TatoHyperRelationsNode *it = *thiss;
	TatoHyperRelationsNode *prev = NULL;

	do {
		// if the item is already present
		if (it->relation == node->relation) {
			return false;
		}
		prev = it;
		it = it->next;

	} while (it != NULL); 

	prev->next = node;
	return true;
}

bool tato_hyper_relations_delete(
	TatoHyperRelations *thiss,
	TatoHyperRelation *relation
) {
	TatoHyperRelationsNode *prev = NULL;
	TatoHyperRelationsNode *it;
	TATO_HYPER_RELATIONS_FOREACH(*thiss, it) {
		if (it->relation == relation) {
			if (prev)
				prev->next = it->next;
			else
				*thiss = it->next;
			tato_hyper_relations_node_free(it);
			return true;
		}
		prev = it;
	}
	return false;
}

bool tato_hyper_relations_related_with(
	TatoHyperRelations thiss,
	struct TatoHyperItemLang_ *lang
) {
	TatoHyperRelationsNode *it;
	TATO_HYPER_RELATIONS_FOREACH(thiss, it) {
		TatoHyperItemsNode *it2;
		TATO_HYPER_ITEMS_FOREACH(it->relation->ends, it2) {
			if (it2->item->lang == lang)
				return true;
		}
	}
	return false;


}

void tato_hyper_relations_debug(TatoHyperRelations thiss) {
	TatoHyperRelationsNode *it;
	TATO_HYPER_RELATIONS_FOREACH(thiss, it) {
		printf("=== HyperRelationNode ===\n");
		tato_hyper_relation_debug(it->relation);
	}
}


