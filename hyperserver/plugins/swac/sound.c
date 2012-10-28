#include "sound.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lang.h"

Sound *sound_new(SoundId id, Lang *lang, char const *str, char const *filename, char const *path, char const *speaker_name, char const *speaker_country, char const *speaker_region) {
	Sound *this = malloc(sizeof(Sound));
	this->id = id;
	this->lang = lang;
	this->str = strdup(str);
	this->filename = strdup(filename);

	this->path = path;
	this->speaker_name = speaker_name;
	this->speaker_country = speaker_country;
	this->speaker_region = speaker_region;
	return this;
}

void sound_free(Sound *this) {
	free(this->str);
	free(this->filename);
	free(this);
}

void sound_debug(Sound *this) {
	printf("== Sound ==\n");
	printf("id: %i\n", this->id);
	printf("lang: %s\n", this->lang->code);
	printf("str: %s\n", this->str);
	printf("filename: %s\n", this->filename);
	printf("path: %s\n", this->path);
	printf("speaker_name: %s\n", this->speaker_name);
	printf("speaker_country: %s\n", this->speaker_country);
	printf("speaker_region: %s\n", this->speaker_region);
}

#define ESCAPE(str) (char *) evhttp_htmlescape(str);

void sound_output_xml(Sound *this, struct evbuffer *buffer) {
	char *str = ESCAPE(this->str);
	char *path = ESCAPE(this->path);
	char *filename = ESCAPE(this->filename);
	char *speaker_name = ESCAPE(this->speaker_name);
	char *speaker_country = ESCAPE(this->speaker_country);
	char *speaker_region = ESCAPE(this->speaker_region);

	evbuffer_add_printf(buffer, "<sound id=\"%i\" lang=\"%s\" str=\"%s\" path=\"%s\" filename=\"%s\" speak_name=\"%s\" speak_lang_country=\"%s\" speak_lang_region=\"%s\"/>\n",
		this->id,
		this->lang->code,
		str,
		path,
		filename,
		speaker_name,
		speaker_country,
		speaker_region
	);

	free(str);
	free(path);
	free(filename);
	free(speaker_name);
	free(speaker_country);
	free(speaker_region);
}
	

