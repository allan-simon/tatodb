#ifndef _TATO_DATA_PARSER_H
#define _TATO_DATA_PARSER_H

#include <expat.h>
#include <stdbool.h>

#include "db.h"

typedef struct {
	XML_Parser handle;
	TatoItem *item;
	TatoRelation *relation;
	TatoDb *db;
} TatoDataParser;

TatoDataParser *tato_data_parser_new(TatoDb *db);
void tato_data_parser_free(TatoDataParser *thiss);
bool tato_data_parser_read(TatoDataParser *thiss, char const *path);

#endif
