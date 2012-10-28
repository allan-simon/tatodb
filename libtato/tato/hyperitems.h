#ifndef _TATO_HYPER_ITEMS_H
#define _TATO_HYPER_ITEMS_H

#include <stdbool.h>

#include "tato.h"

/**
 * Linked list for items
 *
 */
typedef struct TatoHyperItemsNode_ {
	struct TatoHyperItem_ *item;
	struct TatoHyperItemsNode_ * next;
} TatoHyperItemsNode;

typedef TatoHyperItemsNode *TatoHyperItems;

/**
 * Create a new linked-list of TatoHyperItems with the one in argument as the 
 * first one
 * return a pointer on the newly created linked-list 
 */
TatoHyperItemsNode *tato_hyper_items_node_new(struct TatoHyperItem_ *item);

/**
 * TODO: write doc
 */
void tato_hyper_items_node_free(TatoHyperItemsNode *thiss);

/**
 * TODO: write doc
 */
void tato_hyper_items_clear(TatoHyperItems *thiss);

/**
 * Remove an hyperitem from the linked list
 * return true if it could have been removed, false otherwise
 */
bool tato_hyper_items_delete(TatoHyperItems *thiss, struct TatoHyperItem_ *item);

/**
 * Add a new hyperitem node to the linked list
 * duplicate are not added
 */
bool tato_hyper_items_add(TatoHyperItems *thiss, TatoHyperItemsNode *node);

/**
 * Print the internal structure of the given linked-list of Hyperitems
 * on the standard output
 */
void tato_hyper_items_debug(TatoHyperItems thiss);

#define TATO_HYPER_ITEMS_FOREACH(thiss, it) for (it = thiss; it != NULL; it = it->next)

#endif
