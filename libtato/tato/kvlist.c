#include "kvlist.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xml.h"

TatoKvListNode *kvlist_node_new(char const *key, char const *value) {
	TatoKvListNode *thiss = malloc(sizeof(TatoKvListNode));
	thiss->key = key;
	thiss->value = strdup(value);
	thiss->next = NULL;
	return thiss;
}

void tato_kvlist_node_free(TatoKvListNode *thiss) {
	free(thiss->value);
	free(thiss);
}

void tato_kvlist_node_dump(TatoKvListNode *thiss, FILE *f, char const *name) {
	char *key = (char *) tato_xml_escape(thiss->key);
	char *value = (char *) tato_xml_escape(thiss->value);
	fprintf(f, "<%s key=\"%s\" value=\"%s\"/>\n", name, key, value);
	free(key);
	free(value);
}

/**
 *
 */
void tato_kvlist_node_sphinx_dump(TatoKvListNode *thiss, FILE *f) {
    //TODO key may be not xml tag name compliant (for example may contain a space etc.)
	char *key = (char *) tato_xml_escape(thiss->key);
	//char *value = (char *) tato_xml_escape(thiss->value);
	fprintf(f, "<%s><![CDATA[%s]]></%s>\n", key, thiss->value, key);
	free(key);
}



void tato_kvlist_clear(TatoKvList *thiss) {
	TatoKvListNode *it, *next;
	for (it = *thiss; it != NULL; it = next) {
		next = it->next;
		tato_kvlist_node_free(it);
	}
	*thiss = NULL;
}

bool tato_kvlist_add(TatoKvList *thiss, char const *key, char const *value) {
	if (*thiss == NULL) {
		*thiss = kvlist_node_new(key, value);
		return true;
	}
	TatoKvListNode *it, *last;
	TATO_KVLIST_FOREACH(*thiss, it) {
		if (strcmp(it->key, key) == 0)
			return false;
		last = it;
	}

	last->next = kvlist_node_new(key, value);
	return true;
}

char const *tato_kvlist_get(TatoKvList thiss, char const *key) {
	TatoKvListNode *it;
	TATO_KVLIST_FOREACH(thiss, it) {
		if (strcmp(it->key, key) == 0)
			return it->value;
	}
	return NULL;
}

bool tato_kvlist_set(TatoKvList thiss, char const *key, char const *value) {
	TatoKvListNode *it;
	TATO_KVLIST_FOREACH(thiss, it) {
		if (strcmp(it->key, key) == 0) {
			free(it->value);
			it->value = strdup(value);
			return true;
		}
	}
	return false;
}

bool tato_kvlist_delete(TatoKvList *thiss, char const *key) {
	TatoKvListNode *it, *prev = NULL;
	TATO_KVLIST_FOREACH(*thiss, it) {
		if (strcmp(it->key, key) == 0) {
			if (prev)
				prev->next = it->next;
			else
				*thiss = it->next;
			tato_kvlist_node_free(it);
			return true;
		}
		prev = it;
	}
	return false;
}

/**
 *
 */
void tato_kvlist_sphinx_dump(TatoKvList thiss, FILE *f) {
	TatoKvListNode *it;
	TATO_KVLIST_FOREACH(thiss, it) {
		tato_kvlist_node_sphinx_dump(it, f);
	}
}



void tato_kvlist_dump(TatoKvList thiss, FILE *f, char const *name) {
	TatoKvListNode *it;
	TATO_KVLIST_FOREACH(thiss, it) {
		tato_kvlist_node_dump(it, f, name);
	}
}

void tato_kvlist_debug(TatoKvList thiss) {
	printf("=== META ===\n");
	TatoKvListNode *it;
	TATO_KVLIST_FOREACH(thiss, it) {
		printf("%s = %s\n", it->key, it->value);
	}
}
