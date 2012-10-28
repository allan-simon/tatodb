#ifndef _TATO_HYPERRELATION_H
#define _TATO_HYPERRELATION_H

#include <stdbool.h>

#include "tato.h"

#include "hyperitem.h"
#include "hyperitems.h"
#include "kvlist.h"

typedef unsigned int TatoHyperRelationId;
typedef unsigned int TatoHyperRelationFlags;
typedef unsigned int TatoHyperRelationType;


/**
 * This structure represent a particular kind of hyperedge
 * it's an oriented hyperedge with 1 start and one or several ends
 * 
 * id is the unique identifier among other hyperedge
 * start is the starting node
 * ends is a linked-list of the ending nodes
 * type is the kind of hyperedge
 * flags is the list of flags set on it
 * metas is a key/value array of the metas associated
 *
 */

typedef struct TatoHyperRelation_ {
	TatoHyperRelationId id;
	TatoHyperItem *start;
	TatoHyperItems ends;
	TatoHyperRelationType type;
	TatoHyperRelationFlags flags;
	TatoKvList metas;
} TatoHyperRelation;

/**
 * Instantiate a new hyper edge with a start and a end
 */
TatoHyperRelation *tato_hyper_relation_new(
	TatoHyperRelationId id,
	TatoHyperItem *start,
	TatoHyperItem *end,
	TatoHyperRelationType type,
	TatoHyperRelationFlags flags
);
/**
 * Free the given hyper relation
 */
void tato_hyper_relation_free(TatoHyperRelation *thiss);

/**
 * Remove the given hyper relation and adapt the items linked to it
 * and free it
 */
void tato_hyper_relation_delete(TatoHyperRelation *thiss);

/**
 * Disconnect the given hyper relation from all its hyper items.
 */
void tato_hyper_relation_unlink_all(TatoHyperRelation *thiss);

/**
 * Remove the given hyper item from the list of ending hyper items 
 * of the given hyper edge
 */
void tato_hyper_relation_unlink_one(
	TatoHyperRelation *thiss,
	TatoHyperItem *item
);

/**
 * Replace in the given hyper relation
 * the 2nd arg hyper item by the 3rd arg one
 */
void tato_hyper_relation_link_change_start(
	TatoHyperRelation *thiss,
	TatoHyperItem *olditem,
	TatoHyperItem *newitem
);

void tato_hyper_relation_link_change_end(
	TatoHyperRelation *thiss,
	TatoHyperItem *olditem,
	TatoHyperItem *newitem
);



/**
 * Add the given hyper item in the list of "ends" of the given hyper relation 
 */
void tato_hyper_relation_link_add(TatoHyperRelation *thiss, TatoHyperItem *newitem);

void tato_hyper_relation_flags_set(TatoHyperRelation *thiss, TatoHyperRelationFlags const flags);

void tato_hyper_relation_type_set(TatoHyperRelation *thiss, TatoHyperRelationType const type);

bool tato_hyper_relation_meta_add(TatoHyperRelation *thiss, char const *key, char const *value);
char const *tato_hyper_relation_meta_get(TatoHyperRelation *thiss, char const *key);
bool tato_hyper_relation_meta_set(TatoHyperRelation *thiss, char const *key, char const *value);
bool tato_hyper_relation_meta_delete(TatoHyperRelation *thiss, char const *key);

void tato_hyper_relation_debug(TatoHyperRelation *thiss);
void tato_hyper_relation_dump(TatoHyperRelation *thiss, FILE *f);

/**
 * All the callback use for filtering will have this signature
 * 
 * 1st arg: pointer on the data we want to transfer
 * 2nd arg: the hyper relation being evaluated
 * 3rd arg: the hyper items at the other side of the hyper relation
 *
 * it returns a bool, which can be use to control any hypergraph traversal algo
 */
typedef bool (*TatoHyperRelationFilterCallback)(void *, TatoHyperRelation *, TatoHyperItems *);

typedef struct TatoHyperRelationFilter_ {
	TatoHyperRelationFilterCallback callback;
	void *data;
} TatoHyperRelationFilter;

/**
 * Create a new TatoHyperRelationFilter with the given callback and data
 * and return a pointer on it.
 */
TatoHyperRelationFilter *tato_hyper_relation_filter_new(
	TatoHyperRelationFilterCallback callback,
	void *data
);

/**
 * Initialize the fields of a newly created TatoHyperRelationFilter
 * to the given callback and data
 */
void tato_hyper_relation_filter_init(
	TatoHyperRelationFilter *thiss,
	TatoHyperRelationFilterCallback callback,
	void *data
);
/**
 * Free the given TatoHyperRelationFilter
 */
void tato_hyper_relation_filter_free(TatoHyperRelationFilter *thiss);
/**
 * Apply the filter to the given hyper relation
 * return a bool
 */
bool tato_hyper_relation_filter_eval(
	TatoHyperRelationFilter *thiss,
	TatoHyperRelation *relation,
	TatoHyperItems *withs
);


#endif

