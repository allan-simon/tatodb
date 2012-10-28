#ifndef _TATO_HYPER_DATA_PARSER_H
#define _TATO_HYPER_DATA_PARSER_H

#include <expat.h>
#include <stdbool.h>

#include "hyperdb.h"

typedef struct {
	XML_Parser handle;
	TatoHyperItem *item;
	TatoHyperRelation *relation;
	TatoHyperDb *db;
} TatoHyperDataParser;

TatoHyperDataParser *tato_hyper_data_parser_new(TatoHyperDb *db);
void tato_hyper_data_parser_free(TatoHyperDataParser *thiss);
bool tato_hyper_data_parser_read(TatoHyperDataParser *thiss, char const *path);

#endif
