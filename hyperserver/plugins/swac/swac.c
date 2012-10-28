#include "swac.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tato/log.h>

#include "sound.h"

//== SQlite3 helpers ==

void sql_error(sqlite3 *db) {
	tato_log_msg_printf(LOG_ERR, "SQLite error: %s\n", sqlite3_errmsg(db));
}

bool sql_open(sqlite3 **db, char const *path) {
	int rc = sqlite3_open(path, db);
	if (rc) {
		sql_error(*db);
		sqlite3_close(*db);
	}
	return (rc == SQLITE_OK);
}

//== Swac ==
 
Swac *swac_new() {
	Swac *this = malloc(sizeof(Swac));
	this->langs = tato_tree_str_new();
	this->common = tato_tree_str_new();
	this->path = NULL;
	return this;
}

void swac_free(Swac *this) {
	swac_clear(this);
	tato_tree_str_free(this->langs);
	tato_tree_str_free(this->common);
	if (this->path) free(this->path);
	free(this);
}

void swac_clear(Swac *this) {
	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(this->langs, it) {
		lang_free(it->value);
	}
	tato_tree_str_clear(this->langs);

	TATO_TREE_STR_FOREACH(this->common, it) {
		free(it->value);
	}
	tato_tree_str_clear(this->common);
}

bool swac_param(Swac *this, char const *name, char const *value) {
	if (strcmp(name, "path") == 0) {
		this->path = strdup(value);
		return true;
	}
	return false;
}

Lang *swac_lang_find(Swac *this, char const *code) {
	return (Lang *) tato_tree_str_find(this->langs, code);
}

Lang *swac_lang_find_or_create(Swac *this, char const *code) {
	Lang *found = tato_tree_str_find(this->langs, code);
	if (found) {
		return found;
	}
	Lang *lang = lang_new(code);
	tato_tree_str_insert_unique(this->langs, lang->code, lang);
	return lang;
}

char const *swac_common_str(Swac *this, char const *str) {
	TatoTreeStrNode *node = tato_tree_str_node_find(this->common, str);
	if (node) {
		return node->key;
	}

	char const *newstr = strdup(str); 
	tato_tree_str_insert_unique(this->common, newstr, NULL);
	return newstr;
}

bool swac_sounds_load(Swac *this, sqlite3 *db) {
	sqlite3_stmt *stmt;
	int res = sqlite3_prepare(db, "SELECT sounds.idx, swac_text, swac_lang, filename, packages.path, swac_speak_name, swac_speak_lang_country, swac_speak_lang_region FROM sounds INNER JOIN packages ON packages_idx = packages.idx;", -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		sql_error(db);
		return false;
	}

	while ((res = sqlite3_step(stmt)) == SQLITE_ROW) {
		SoundId id = sqlite3_column_int(stmt, 0);
		char const *text = (char const *) sqlite3_column_text(stmt, 1);
		char const *lang_code = (char const *) sqlite3_column_text(stmt, 2);
		char const *filename = (char const *) sqlite3_column_text(stmt, 3);
		char const *path = swac_common_str(this, (char const *) sqlite3_column_text(stmt, 4));
		char const *speaker_name = swac_common_str(this, (char const *) sqlite3_column_text(stmt, 5));
		char const *speaker_country = swac_common_str(this, (char const *) sqlite3_column_text(stmt, 6));
		char const *speaker_region = swac_common_str(this, (char const *) sqlite3_column_text(stmt, 7));

		Lang *lang = swac_lang_find_or_create(this, lang_code);
		Sound *sound = sound_new(id, lang, text, filename, path, speaker_name, speaker_country, speaker_region);
		lang_sound_add(lang, sound);
	}
	sqlite3_finalize(stmt);
	return true;
}

bool swac_load(Swac *this, char const *path) {
	sqlite3 *db;
	if (!sql_open(&db, path)) {
		return false;
	}

	return swac_sounds_load(this, db);
}

bool swac_start(Swac *this) {
	if (!this->path)
		return false;

	tato_log_msg_printf(LOG_NOTICE, "Loading data from \"%s\"", this->path);
	bool res = swac_load(this, this->path);
	return res;
}

bool swac_reload(Swac *this, char const *path) {
	swac_clear(this);
	return swac_load(this, path);
}

void swac_output_stats(Swac *this, struct evbuffer *buffer) {
	evbuffer_add_printf(buffer, "LANGS %i\n", tato_tree_str_size(this->langs));
	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(this->langs, it) {
		Lang *lang = it->value;
		evbuffer_add_printf(buffer, "LANG[\"%s\"] %i\n", lang->code, tato_tree_str_size(lang->index));
	}
}

