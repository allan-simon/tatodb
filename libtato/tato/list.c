#include "list.h"

#include <stdlib.h>

TatoList *tato_list_new() {
	TatoList *thiss = malloc(sizeof(TatoList));
	thiss->next = thiss;
	thiss->previous = thiss;
	thiss->data = TATO_LIST_DATA_NULL;
	return thiss;
}

void tato_list_free(TatoList *thiss) {
	TatoListNode *it;
	TATO_LIST_FOREACH(thiss, it) {
		free(it);
	}
	free(thiss);
}

bool tato_list_empty(TatoList *thiss) {
	return TATO_LIST_EMPTY(thiss);
}

TatoListNode *tato_list_unshift(TatoList *thiss, TatoListData data) {
	return tato_list_node_insert_after(thiss, data);
}

TatoListData tato_list_shift(TatoList *thiss) {
	if (TATO_LIST_EMPTY(thiss))
		return TATO_LIST_DATA_NULL; 

	TatoListData data = thiss->previous->data;
	tato_list_node_remove(thiss->previous);
	return data;
}

TatoListNode *tato_list_push(TatoList *thiss, TatoListData data) {
	return tato_list_node_insert_after(thiss, data);
}

TatoListData tato_list_pop(TatoList *thiss) {
	if (TATO_LIST_EMPTY(thiss))
		return TATO_LIST_DATA_NULL; 

	TatoListData data = thiss->next->data;
	tato_list_node_remove(thiss->next);
	return data;
}

TatoListNode *tato_list_node_new(TatoListData data) {
	TatoListNode *thiss = malloc(sizeof(TatoListNode));
	thiss->data = data;
	return thiss;
}

TatoListNode *tato_list_node_insert_before(TatoListNode *thiss, TatoListData data) {
	TatoListNode *node = tato_list_node_new(data);
	node->previous = thiss->previous;
	node->next = thiss;
	thiss->previous->next = node; 
	thiss->previous = node;
	return node;
}

TatoListNode *tato_list_node_insert_after(TatoListNode *thiss, TatoListData data) {
	TatoListNode *node = tato_list_node_new(data);
	node->previous = thiss;
	node->next = thiss->next;
	thiss->next->previous = node; 
	thiss->next = node;
	return node;
}

bool tato_list_node_remove(TatoListNode *thiss) {
	if (TATO_LIST_EMPTY(thiss))
		return false;

	thiss->previous->next = thiss->next;
	thiss->next->previous = thiss->previous;
	free(thiss);
	return true;
}

