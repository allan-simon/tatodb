#ifndef _TATO_LANG_H
#define _TATO_LANG_H

#include "tato.h"

#include "tree_str.h"
#include "fetcher.h"


#define TATO_LANG_GENERATE_INCLUDE(ItemType, item_prefix) \
	typedef struct Tato##ItemType##Lang_ {\
		char *code;\
		TatoTreeStr *index;\
	} Tato##ItemType##Lang;\
	\
	\
	Tato##ItemType##Lang *tato_##item_prefix##_lang_new(char const *code);\
	void tato_##item_prefix##_lang_free(Tato##ItemType##Lang *thiss);\
	void tato_##item_prefix##_lang_debug(Tato##ItemType##Lang *thiss);\
	\
	int tato_##item_prefix##_lang_item_can_add(\
		Tato##ItemType##Lang *thiss,\
		char const *str\
	); \
	void tato_##item_prefix##_lang_item_add(\
		Tato##ItemType##Lang *thiss,\
		struct Tato##ItemType##_ *item\
	); \
	void tato_##item_prefix##_lang_item_remove(\
		Tato##ItemType##Lang *thiss,\
		struct Tato##ItemType##_ *item\
	); \
	struct Tato##ItemType##_ *tato_##item_prefix##_lang_item_find(\
		Tato##ItemType##Lang *thiss,\
		char const *str\
	);\
	\
	void tato_##item_prefix##_lang_items_related_with(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		Tato##ItemType##Lang *lang,\
		bool inversed\
	);\
	\
	void tato_##item_prefix##_lang_items_get_seq(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher\
	);\
	\
	void tato_##item_prefix##_lang_items_search(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		char const *str\
	);\
	void tato_##item_prefix##_lang_items_suggest(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		char const *str\
	);\
	\
	void tato_##item_prefix##_lang_items_suggest_pagi(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		char const *str\
	);\




#define TATO_LANG_GENERATE_BODY(ItemType, item_prefix) \
	int tato_##item_prefix##_lang_item_can_add(Tato##ItemType##Lang *thiss, char const *str) {\
		/*avec dédoublonnage*/\
		Tato##ItemType *item = tato_tree_str_find(thiss->index, str);\
		return !item ? -1 : item->id;\
	}\
	\
	void tato_##item_prefix##_lang_item_add(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType *item\
	) {\
		tato_tree_str_insert(thiss->index, item->str, item);\
	}\
	\
	void tato_##item_prefix##_lang_item_remove(Tato##ItemType##Lang *thiss, Tato##ItemType *item) { \
		tato_tree_str_remove(thiss->index, item->str, item);\
	}\
	\
	Tato##ItemType *tato_##item_prefix##_lang_item_find(Tato##ItemType##Lang *thiss, char const *str) {\
		 return tato_tree_str_find(thiss->index, str);\
	}\
	void tato_##item_prefix##_lang_items_related_with(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		Tato##ItemType##Lang *lang,\
		bool inversed\
	) {\
		TatoTreeStrNode *it = NULL;\
		TATO_TREE_STR_FOREACH(thiss->index, it) {\
			Tato##ItemType *item = it->value;\
			if (tato_##item_prefix##_related_with(item, lang) ^ inversed)\
				tato_##item_prefix##_fetcher_add(fetcher, item);\
			if (tato_##item_prefix##_fetcher_eof(fetcher))\
				return;\
		}\
	}\
	\
	void tato_##item_prefix##_lang_items_get_seq(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher\
	) {\
		TatoTreeStrNode *it = NULL;\
		fetcher->max_size = tato_tree_str_size(thiss->index);\
		TATO_TREE_STR_FOREACH(thiss->index, it) {\
			Tato##ItemType *item = it->value;\
			tato_##item_prefix##_fetcher_add(fetcher, item);\
			if (tato_##item_prefix##_fetcher_eof(fetcher))\
				return;\
		}\
	}\
	\
	void tato_##item_prefix##_lang_items_search(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		char const *str\
	) {\
		TatoTreeStrNode *it;\
		TATO_TREE_STR_FOREACH_SIMILAR(thiss->index, it, str) {\
			tato_##item_prefix##_fetcher_add(fetcher, it->value);\
		}\
	}\
	\
	void tato_##item_prefix##_lang_items_suggest(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		char const *str\
	) {\
		TatoTreeStrNode *it = tato_tree_str_node_nfind(thiss->index, str);\
		while (it) {\
			if (strncmp(str, it->key, strlen(str)) != 0)\
				return;\
			\
			tato_##item_prefix##_fetcher_add(fetcher, it->value);\
			if (tato_##item_prefix##_fetcher_eof(fetcher))\
				return;\
			it = tato_tree_str_node_next(it);\
		}\
	}\
	\
	\
	void tato_##item_prefix##_lang_items_suggest_pagi(\
		Tato##ItemType##Lang *thiss,\
		Tato##ItemType##Fetcher *fetcher,\
		char const *str\
	) {\
		TatoTreeStrNode *it = tato_tree_str_node_nfind(thiss->index, str);\
		while (it) {\
			if (strncmp(str, it->key, strlen(str)) != 0)\
				return;\
			\
			if (!tato_##item_prefix##_fetcher_eof(fetcher)) {\
				tato_##item_prefix##_fetcher_add(fetcher, it->value);\
			}\
			fetcher->max_size++;\
			it = tato_tree_str_node_next(it);\
		}\
	}\
	\
	Tato##ItemType##Lang *tato_##item_prefix##_lang_new(char const *code) {\
		Tato##ItemType##Lang *thiss = malloc(sizeof(Tato##ItemType##Lang));\
		thiss->code = strdup(code);\
		thiss->index = tato_tree_str_new();\
		return thiss;\
	}\
	\
	void tato_##item_prefix##_lang_free(Tato##ItemType##Lang *thiss) {\
		free(thiss->code);\
		tato_tree_str_free(thiss->index);\
		free(thiss);\
	}\
	\
	void tato_##item_prefix##_lang_debug(Tato##ItemType##Lang *thiss) {\
		printf("== Tato##ItemType##Lang ==\n");\
		printf("code: %s\n", thiss->code);\
		printf("index.size: %i\n", tato_tree_str_size(thiss->index));\
		/*tato_tree_str_debug(thiss->index);*/\
	}

TATO_LANG_GENERATE_INCLUDE(Item, item)
TATO_LANG_GENERATE_INCLUDE(HyperItem, hyper_item)

#endif

