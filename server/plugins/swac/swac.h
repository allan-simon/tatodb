#ifndef _SWAC_H
#define _SWAC_H

#include <stdbool.h>
#include <sqlite3.h>
#include <evhttp.h>

#include <tato/tree_str.h>

#include "lang.h"

typedef struct Swac_ {
	TatoTreeStr *common;
	TatoTreeStr *langs;
	char *path;
} Swac;

Swac *swac_new();
void swac_free(Swac *this);
void swac_clear(Swac *this);
bool swac_param(Swac *this, char const *name, char const *value);
bool swac_start(Swac *this);
Lang *swac_lang_find(Swac *this, char const *code);

char const *swac_common_str(Swac *this, char const *str);

Lang *swac_lang_find(Swac *this, char const *code);
Lang *swac_lang_find_or_create(Swac *this, char const *code);

bool swac_sounds_load(Swac *this, sqlite3 *db);
bool swac_load(Swac *this, char const *path);
bool swac_reload(Swac *this, char const *path);
void swac_output_stats(Swac *this, struct evbuffer *buffer);

#endif
