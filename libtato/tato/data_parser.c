#include "data_parser.h"

#include <stdio.h>
#include <string.h>

#include "log.h"
#include "item.h"
#include "relation.h"

#define BUFFER_SIZE 4096

static XML_Char *get_attr(XML_Char const **attr, XML_Char const *name) {
	int i;
	for (i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], name) == 0)
			return (XML_Char *) attr[i + 1];
	}
	return NULL;
}

static void element_start(void *data, XML_Char const *name, XML_Char const **attr) {
	TatoDataParser *thiss = (TatoDataParser *) data;

	//<item>
	if (strcmp(name, "item") == 0) {
		//if a <rel> or <item> was already opened 
		if (thiss->item || thiss->relation) {
			tato_log_msg_printf(LOG_ERR, "could not open <rel> here");
			return;
		}

		char *id = get_attr(attr, "id");
		char *lang = get_attr(attr, "lang");
		char *str = get_attr(attr, "str");
		char *flags = get_attr(attr, "flags");
		if (!id || !lang || !str) {
			return;
		}

		TatoItem *item = tato_db_item_new(thiss->db, atoi(id), lang, str, flags ? atoi(flags) : 0);
		if (!item) {
			tato_log_msg_printf(LOG_ERR, "Could not create item %i (\"%s\")", item->id, item->str);
		}
		else {
			thiss->item = item;
		}
	}

	//<rel>
	else if (strcmp(name, "rel") == 0) {
		//if a <rel> or <item> was already opened 
		if (thiss->item || thiss->relation) {
			tato_log_msg_printf(LOG_ERR, "could not open <rel> here");
			return;
		}

		char *id = get_attr(attr, "id");
		char *x = get_attr(attr, "x");
		char *y = get_attr(attr, "y");
		if (!id || !x || !y) {
			return;
		}
		char *type = get_attr(attr, "type");
		char *flags = get_attr(attr, "flags");

		TatoRelation *relation = tato_db_relation_new(thiss->db, atoi(id), atoi(x), atoi(y), type ? atoi(type) : 0, flags ? atoi(flags) : 0);
		if (!relation) {
			tato_log_msg_printf(LOG_ERR, "Could not create relation %i", relation->id);
		}
		else {
			thiss->relation = relation;
		}
	}

	//<meta>
	else if (strcmp(name, "meta") == 0) {
		if (!thiss->item && !thiss->relation) {
			tato_log_msg_printf(LOG_ERR, "<meta> defined out of a <item> or a <rel> node");
			return;
		}

		char *key = get_attr(attr, "key");
		char *value = get_attr(attr, "value");
		if (!key || !value) {
			return;
		}

		if (thiss->item) {
			tato_item_meta_add(thiss->item, tato_db_common_str(thiss->db, key), value);
		}
		else if (thiss->relation) {
			tato_relation_meta_add(thiss->relation, tato_db_common_str(thiss->db, key), value);
		}
	}
}

static void element_end(void *data, XML_Char const *name) {
	TatoDataParser *thiss = (TatoDataParser *) data;

	//</item>
	if (thiss->item && strcmp(name, "item") == 0) {
		thiss->item = NULL;
	}

	//</rel>
	else if (thiss->relation && strcmp(name, "rel") == 0) {
		thiss->relation = NULL;
	}
}

static void error_print(XML_Parser handle) {
	int code = XML_GetErrorCode(handle);
	int col = XML_GetCurrentColumnNumber(handle);
	int line = XML_GetCurrentLineNumber(handle);
	tato_log_msg_printf(LOG_ERR, "Expat error (%i) on (%i:%i): %s", code, line, col, XML_ErrorString(code));
}

void tato_data_parser_init(TatoDataParser *thiss) {
	thiss->handle = XML_ParserCreate("utf-8");
	XML_SetUserData(thiss->handle, thiss);
	XML_SetElementHandler(thiss->handle, element_start, element_end);
}

void tato_data_parser_clear(TatoDataParser *thiss) {
	XML_ParserFree(thiss->handle);
}

TatoDataParser *tato_data_parser_new(TatoDb *db) {
	TatoDataParser *thiss = malloc(sizeof(TatoDataParser));
	tato_data_parser_init(thiss);
	thiss->db = db;
	thiss->item = NULL;
	thiss->relation = NULL;
	return thiss;
}

void tato_data_parser_free(TatoDataParser *thiss) {
	tato_data_parser_clear(thiss);
	free(thiss);
}

bool tato_data_parser_read(TatoDataParser *thiss, char const *path) {
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		tato_log_msg_printf(LOG_ERR, "Could not read input file");
		return false;
	}

	tato_log_msg_printf(LOG_NOTICE, "Loading data from \"%s\"", path);
	void *buffer = malloc(BUFFER_SIZE);
	for (;;) {
		size_t readen = fread(buffer, 1, BUFFER_SIZE, f);
		int last = readen != BUFFER_SIZE ? 1 : 0;

		if (XML_Parse(thiss->handle, buffer, readen, last) == 0) {
			error_print(thiss->handle);

			free(buffer);
			fclose(f);
			return false;
		}
		if (last)
			break;
	}

	fclose(f);
	free(buffer);
	return true;
}

