#ifndef _LANG_H
#define _LANG_H

#include <tato/tree_str.h>

#include "sound.h"

typedef struct Lang_ {
	char *code;
	TatoTreeStr *index;
} Lang;

Lang *lang_new(char const *code);
void lang_free(Lang *this);
void lang_sound_add(Lang *this, Sound *sound);
void lang_sound_remove(Lang *this, Sound *sound);
Sound *lang_sound_find(Lang *this, char const *str);
void lang_debug(Lang *this);

#endif
