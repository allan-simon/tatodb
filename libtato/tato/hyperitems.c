#include "hyperitems.h"

#include <stdlib.h>
#include <stdio.h>

#include "hyperitem.h"
#include "lang.h"

/**
 * TODO the code is 90% the same than the one in relations.c
 */

TatoHyperItemsNode *tato_hyper_items_node_new(TatoHyperItem *item) {
	TatoHyperItemsNode *thiss = malloc(sizeof(TatoHyperItemsNode));
	thiss->item = item;
	thiss->next = NULL;
	return thiss;
}

void tato_hyper_items_node_free(TatoHyperItemsNode *thiss) {
	free(thiss);
}

void tato_hyper_items_clear(TatoHyperItems *thiss) {
	TatoHyperItemsNode *it, *next;

	for (it = *thiss; it != NULL; it = next) {
		next = it->next;
		tato_hyper_items_node_free(it);
	}
	*thiss = NULL;
}

bool tato_hyper_items_add(TatoHyperItems *thiss, TatoHyperItemsNode *node) {
	if (*thiss == NULL) {
		*thiss = node;
		return true;
	}
	TatoHyperItemsNode *it = *thiss;
	TatoHyperItemsNode *prev = NULL;

	do {
		// if the item is already present
		if (it->item == node->item) {
			return false;
		}
		prev = it;
		it = it->next;
	} while (it != NULL); 
	prev->next = node;
	return true;
}

bool tato_hyper_items_delete(TatoHyperItems *thiss, TatoHyperItem *item) {
	TatoHyperItemsNode *prev = NULL;
	TatoHyperItemsNode *it;
	TATO_HYPER_ITEMS_FOREACH(*thiss, it) {
		if (it->item == item) {
			if (prev)
				prev->next = it->next;
			else
				*thiss = it->next;
			tato_hyper_items_node_free(it);
			return true;
		}
		prev = it;
	}
	return false;
}

void tato_hyper_items_debug(TatoHyperItems thiss) {
	TatoHyperItemsNode *it;
	TATO_HYPER_ITEMS_FOREACH(thiss, it) {
		printf("=== HyperItemNode ===\n");
		tato_hyper_item_debug(it->item);
	}
}

