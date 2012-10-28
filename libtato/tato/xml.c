#include "xml.h"

#include <stdio.h>
#include <stdlib.h>

const char *tato_xml_escape_char(char c) {
	switch (c) {
		case '<':
			return "&lt;";
		case '>':
			return "&gt;";
		case '"':
			return "&quot;";
		case '\'':
			return "&#039;";
		case '&':
			return "&amp;";
		default:
			break;
	}
	return NULL;
}

char *tato_xml_escape(const char *str) {
	const char *replaced;
	char const *it;

	// 1 not 0 because the size is "size" + '\0'
	size_t size = 1;
	for (it = str; *it != '\0'; it++) {
		replaced = tato_xml_escape_char(*it);
		size += replaced ? strlen(replaced) : 1;
	}

	char *escaped = malloc(size);
	char *cursor = escaped;

	for (it = str; *it != '\0'; it++) {
		replaced = tato_xml_escape_char(*it);

		if (replaced) {
			strcpy(cursor, replaced);
			cursor += strlen(replaced);
		}
		else {
			*cursor = *it;
			cursor++;
		}
	}
	*cursor = '\0';
	return escaped;
}

