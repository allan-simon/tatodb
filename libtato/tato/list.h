#ifndef _TATO_LIST_H
#define _TATO_LIST_H

#include <stdbool.h>

typedef void *TatoListData;
#define TATO_LIST_DATA_NULL NULL

typedef struct TatoListNode_ {
	struct TatoListNode_ *previous;
	struct TatoListNode_ *next;
	TatoListData data;
} TatoListNode;

typedef TatoListNode TatoList;

TatoList *tato_list_new();
void tato_list_free(TatoList *thiss);
bool tato_list_empty(TatoList *thiss);
TatoListNode *tato_list_push(TatoList *thiss, TatoListData data);
TatoListData tato_list_pop(TatoList *thiss);
TatoListNode *tato_list_unshift(TatoList *thiss, TatoListData data);
TatoListData tato_list_shift(TatoList *thiss);

TatoListNode *tato_list_node_insert_before(TatoListNode *thiss, TatoListData data);
TatoListNode *tato_list_node_insert_after(TatoListNode *thiss, TatoListData data);
bool tato_list_node_remove(TatoListNode *thiss);

#define TATO_LIST_FOREACH(thiss, it) for (it = thiss->next; it != thiss; it = it->next)
#define TATO_LIST_FOREACH_REVERSE(thiss, it) for (it = thiss->previous; it != thiss; it = it->previous)
#define TATO_LIST_EMPTY(thiss) (thiss == thiss->next)

#endif
