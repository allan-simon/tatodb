#include "plugin_verbiste.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <verbiste/c-api.h>
#include <tato/item.h>
#include <tato/log.h>

PluginDef plugin_def = {
	"Verbiste Plugin",
	"Plugin for french conjugations",
	"0.1",
	plugin_new,
	plugin_free,
	plugin_start,
	plugin_param,
	plugin_item_join,
	plugin_output_stats
};

typedef struct {
	char *path;
} PluginVerbiste;

void *plugin_new() {
	PluginVerbiste *this = malloc(sizeof(PluginVerbiste));
	this->path = NULL;
	return this;
}

void plugin_free(void *data) {
	PluginVerbiste *this = (PluginVerbiste *) data;

	if (this->path) free(this->path);
	free(this);
	verbiste_close();
}

bool plugin_param(void *data, char const *name, char const *value) {
	PluginVerbiste *this = (PluginVerbiste *) data;
	if (strcmp(name, "path") == 0) {
		this->path = strdup(value);
		return true;
	}
	return false;
}

bool plugin_start(void *data) {
	PluginVerbiste *this = (PluginVerbiste *) data;
	if (!this->path) {
		tato_log_msg_printf(LOG_NOTICE, "Loading verbiste error: not a valid path");
		return false;
	}

	tato_log_msg_printf(LOG_NOTICE, "Loading verbiste plugin (path: \"%s\")", this->path);
	
	char conjFN[512], verbsFN[512];
	snprintf(conjFN, sizeof(conjFN), "%s/conjugation-fr.xml", this->path);
	snprintf(verbsFN, sizeof(verbsFN), "%s/verbs-fr.xml", this->path);
	verbiste_init(conjFN, verbsFN);
	return true;
}

static bool conjugate(const char *infinitive, struct evbuffer *buffer) {
	int k;
	for (k = 0; verbiste_valid_modes_and_tenses[k].mode != VERBISTE_INVALID_MODE; k++) {

		Verbiste_Mode mode = verbiste_valid_modes_and_tenses[k].mode;
		Verbiste_Tense tense = verbiste_valid_modes_and_tenses[k].tense;

		Verbiste_PersonArray person_array = verbiste_conjugate(infinitive, mode, tense, 1);
		if (!person_array)
			return false;

		evbuffer_add_printf(buffer, "<inflection type=\"conjugation\" name=\"%s_%s\">\n", 
			verbiste_get_mode_name(mode), 
			verbiste_get_tense_name(tense)
		);
		
		size_t i;
		for (i = 0; person_array[i] != NULL; i++) {
			size_t j;
			for (j = 0; person_array[i][j] != NULL; j++) {
				char *str = verbiste_latin1_to_utf8(person_array[i][j]);
				evbuffer_add_printf(buffer, "<form str=\"%s\"/>\n", str);
				verbiste_free_string(str);
			}
		}
		verbiste_free_person_array(person_array);
		evbuffer_add_printf(buffer, "</inflection>\n"); 
	}
	return true;
}

void plugin_item_join(void *data, struct evbuffer *buffer, TatoItem *item) {
	if (strcmp(tato_item_lang_code(item), "fra") == 0) {
		char *str = verbiste_utf8_to_latin1(item->str);
		conjugate(str, buffer);
		verbiste_free_string(str);
	}
}

void plugin_output_stats(void *data, struct evbuffer *buffer) {
}

