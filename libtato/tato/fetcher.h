#ifndef _TATO_FETCHER_H
#define _TATO_FETCHER_H

#include <stdlib.h>
#include <stdbool.h>

#include "tato.h"


#define TATO_FETCHER_GENERATE_INCLUDE(ItemType, item_prefix) \
	typedef struct Tato##ItemType##Fetcher_ {\
		size_t offset;\
		size_t skipped;\
		size_t capacity;\
		size_t size;\
		unsigned int max_size;\
		struct Tato##ItemType##_ **items;\
	} Tato##ItemType##Fetcher;\
	\
	Tato##ItemType##Fetcher *tato_##item_prefix##_fetcher_new(\
		size_t capacity,\
		size_t offset\
	);\
	void tato_##item_prefix##_fetcher_free(Tato##ItemType##Fetcher *thiss);\
	bool tato_##item_prefix##_fetcher_add(\
		Tato##ItemType##Fetcher *thiss,\
		struct Tato##ItemType##_ *item\
	);\
	bool tato_##item_prefix##_fetcher_eof(Tato##ItemType##Fetcher *thiss);


#define TATO_FETCHER_GENERATE_BODY(ItemType, item_prefix) \
	Tato##ItemType##Fetcher *tato_##item_prefix##_fetcher_new(\
		size_t capacity,\
		size_t offset\
	) {\
		Tato##ItemType##Fetcher *thiss = malloc(sizeof(Tato##ItemType##Fetcher));\
		thiss->size = 0;\
		thiss->skipped = 0;\
		thiss->max_size = 0;\
		thiss->offset = offset;\
		thiss->capacity = capacity;\
		thiss->items = calloc(capacity, sizeof(struct Tato##ItemType##_ *));\
		return thiss;\
	}\
\
	void tato_##item_prefix##_fetcher_free(Tato##ItemType##Fetcher *thiss) {\
		free(thiss->items);\
		free(thiss);\
	}\
\
	bool tato_##item_prefix##_fetcher_add(\
		Tato##ItemType##Fetcher *thiss,\
		Tato##ItemType *item\
	) {\
		if (thiss->size >= thiss->capacity)\
			return false;\
\
		if (thiss->skipped < thiss->offset) {\
			thiss->skipped++;\
			return true;\
		}\
\
		thiss->items[thiss->size] = item;\
		thiss->size++;\
		return true;\
	}\
\
	bool tato_##item_prefix##_fetcher_eof(Tato##ItemType##Fetcher *thiss) {\
		return (thiss->size >= thiss->capacity);\
	}

TATO_FETCHER_GENERATE_INCLUDE(Item, item)
TATO_FETCHER_GENERATE_INCLUDE(HyperItem, hyper_item)

#endif
