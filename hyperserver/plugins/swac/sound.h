#ifndef _SOUND_H
#define _SOUND_H

#include <evhttp.h>

struct Lang_;

typedef unsigned int SoundId;

typedef struct Sound_ {
	SoundId id;
	struct Lang_ *lang;
	char *str;
	char *filename;

	char const *path;
	char const *speaker_name;
	char const *speaker_country;
	char const *speaker_region;
} Sound;

Sound *sound_new(SoundId id, struct Lang_ *lang, char const *str, char const *filename, char const *path, char const *speaker_name, char const *speaker_country, char const *speaker_region);
void sound_free(Sound *this);
void sound_debug(Sound *this);
void sound_output_xml(Sound *this, struct evbuffer *buffer);

#endif
