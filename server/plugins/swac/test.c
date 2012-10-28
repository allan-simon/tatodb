#include <stdio.h>

#include "swac.h"
#include "lang.h"
#include "sound.h"

int main(int argc, char *argv[]) {
	Swac *swac = swac_new();
	swac_reload(swac, "/home/nico/Projects/web/search/swac.sqlite3");
//	tree_str_debug(swac->langs);
//	tree_str_debug(swac->common);

	Lang *lang = swac_lang_find(swac, "rus");
	if (!lang) {
		printf("Could not find language\n");
		return 0;
	}

	TreeStrNode *it;
	TREE_STR_FOREACH_SIMILAR(lang->index, it, "абрикос") {
		sound_debug(it->value);
	}
}
