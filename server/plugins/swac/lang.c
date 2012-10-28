#include "lang.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lang *lang_new(char const *code) {
	Lang *this = malloc(sizeof(Lang));
	this->code = strdup(code);
	this->index = tato_tree_str_new();
	return this;
}

void lang_free(Lang *this) {
	free(this->code);

	TatoTreeStrNode *it;
	TATO_TREE_STR_FOREACH(this->index, it) {
		sound_free(it->value);
	}
	tato_tree_str_free(this->index);
	free(this);
}

void lang_sound_add(Lang *this, Sound *sound) {
	tato_tree_str_insert(this->index, sound->str, sound);
}

void lang_sound_remove(Lang *this, Sound *sound) {
	tato_tree_str_remove(this->index, sound->str, sound);
}

Sound *lang_sound_find(Lang *this, char const *str) {
	Sound *sound = tato_tree_str_find(this->index, str);
	return sound;
}

void lang_debug(Lang *this) {
	printf("== Lang ==\n");
	printf("code: %s\n", this->code);
	printf("index.size: %i\n", tato_tree_str_size(this->index));
	//tree_str_debug(this->index);
}

