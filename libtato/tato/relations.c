#include "relations.h"

#include <stdlib.h>
#include <stdio.h>

#include "item.h"
#include "relation.h"
#include "lang.h"

TatoRelationsNode *tato_relations_node_new(TatoRelation *relation, TatoItem *with) {
	TatoRelationsNode *thiss = malloc(sizeof(TatoRelationsNode));
	thiss->relation = relation;
	thiss->with = with;
	thiss->next = NULL;
	return thiss;
}

void tato_relations_node_free(TatoRelationsNode *thiss) {
	free(thiss);
}

void tato_relations_clear(TatoRelations *thiss) {
	TatoRelationsNode *it, *next;
	for (it = *thiss; it != NULL; it = next) {
		next = it->next;
		tato_relations_node_free(it);
	}
	*thiss = NULL;
}

bool tato_relations_add(TatoRelations *thiss, TatoRelationsNode *node) {
	if (*thiss == NULL) {
		*thiss = node;
		return true;
	}
	TatoRelationsNode *it = *thiss;
	TatoRelationsNode *prev = NULL;

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

bool tato_relations_delete(TatoRelations *thiss, TatoRelation *relation) {
	TatoRelationsNode *prev = NULL;
	TatoRelationsNode *it;
	TATO_RELATIONS_FOREACH(*thiss, it) {
		if (it->relation == relation) {
			if (prev)
				prev->next = it->next;
			else
				*thiss = it->next;
			tato_relations_node_free(it);
			return true;
		}
		prev = it;
	}
	return false;
}

bool tato_relations_related_with(TatoRelations thiss, TatoItemLang *lang) {
	TatoRelationsNode *it;
	TATO_RELATIONS_FOREACH(thiss, it) {
		if (it->with->lang == lang)
			return true;
	}
	return false;
}

void tato_relations_debug(TatoRelations thiss) {
	TatoRelationsNode *it;
	TATO_RELATIONS_FOREACH(thiss, it) {
		printf("=== RelationNode ===\n");
		tato_relation_debug(it->relation);
	}
}

void tato_relations_unlink(TatoRelations *thiss) {
	while (*thiss)
		tato_relation_unlink((*thiss)->relation);
	*thiss = NULL;
}

