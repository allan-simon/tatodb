#include "item.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "relations.h"
#include "relation.h"
#include "xml.h"

TatoItem *tato_item_new(TatoItemId id, TatoItemLang *lang, char const *str, TatoItemFlags flags) {
	TatoItem *thiss = malloc(sizeof(TatoItem));
	thiss->id = id;
	thiss->str = strdup(str);
	thiss->flags = flags;
	thiss->lang = lang;
	thiss->relations = NULL;
	thiss->metas = NULL;
	return thiss;
}

void tato_item_free(TatoItem *thiss) {
	if (thiss->metas) tato_kvlist_clear(&thiss->metas);
	if (thiss->relations) tato_relations_clear(&thiss->relations);
	if (thiss->str) free(thiss->str);
	thiss->str = NULL;
	free(thiss);	
}

void tato_item_delete(TatoItem *thiss) {
	tato_item_unlink(thiss);
	tato_item_free(thiss);
}

void tato_item_merge_into(TatoItem *thiss, TatoItem *into) {
	TatoRelationsNode *it = NULL;
	TatoRelationsNode *next = NULL;

	// the problem come with "with" of relationNode
	// moreover here the problem that if we merge two direct translation,
	// we will delete a link which is not reported on the database
	if (thiss == into) return;

	for (it = thiss->relations; it != NULL; it = next) {
		next = it->next;
		tato_relation_link_change(it->relation, thiss, into);
	}

	tato_item_delete(thiss);
}

bool tato_item_str_set(TatoItem *thiss, char const *str) {
	if (tato_item_lang_item_can_add(thiss->lang, str) >= 0)
		return false;

	tato_item_lang_item_remove(thiss->lang, thiss);
	free(thiss->str);

	thiss->str = strdup(str);
	tato_item_lang_item_add(thiss->lang, thiss);
	return true;
}

char const *tato_item_lang_code(TatoItem *thiss) {
	return thiss->lang->code;
}

bool tato_item_lang_set(TatoItem *thiss, TatoItemLang *lang) {
	if (tato_item_lang_item_can_add(lang, thiss->str) >=0)
		return false;

	tato_item_lang_item_remove(thiss->lang, thiss);

	thiss->lang = lang;
	tato_item_lang_item_add(lang, thiss);
	return true;
}

void tato_item_flags_set(TatoItem *thiss, TatoItemFlags const flags) {
	thiss->flags = flags;
}

bool tato_item_meta_add(TatoItem *thiss, char const *key, char const *value) {
	return tato_kvlist_add(&thiss->metas, key, value);
}

char const *tato_item_meta_get(TatoItem *thiss, char const *key) {
	return tato_kvlist_get(thiss->metas, key);
}

bool tato_item_meta_set(TatoItem *thiss, char const *key, char const *value) {
	return tato_kvlist_set(thiss->metas, key, value);
}

bool tato_item_meta_delete(TatoItem *thiss, char const *key) {
	return tato_kvlist_delete(&thiss->metas, key);
}

void tato_item_relation_add(TatoItem *thiss, TatoRelation *relation, TatoItem *with) {
	TatoRelationsNode *node = tato_relations_node_new(relation, with);
	if (!tato_relations_add(&(thiss->relations), node)) {
		tato_relations_node_free(node);
	}
}

bool tato_item_relation_remove(TatoItem *thiss, TatoRelation *relation) {
	return tato_relations_delete(&(thiss->relations), relation);
}

void tato_item_unlink(TatoItem *thiss) {
	tato_relations_unlink(&(thiss->relations));
}

bool tato_item_related_with(TatoItem *thiss, TatoItemLang *lang) {
	return (lang == NULL)
		? (thiss->relations == NULL)
		: tato_relations_related_with(thiss->relations, lang);	
}

void tato_item_dump(TatoItem *thiss, FILE *f) {
	bool opened = (thiss->metas != NULL);
	char *str = (char *) tato_xml_escape(thiss->str);
	fprintf(f, "<item id=\"%i\" lang=\"%s\" str=\"%s\" flags=\"%i\"%c>\n",
		thiss->id,
		tato_item_lang_code(thiss),
		str,
		thiss->flags,
		opened ? ' ' : '/'
	);
	free(str);
	if (opened) {
		tato_kvlist_dump(thiss->metas, f, "meta");
		fprintf(f, "</item>\n"); 
	}
}

//TODO move this somewhere else (moreover the variable is never freed)
static const char* get_tato_lang_id(char* code) {
    static TatoKvList isoToId = NULL;
    if (isoToId == NULL) {
        tato_kvlist_add(&isoToId, "acm" , " 2"); 
        tato_kvlist_add(&isoToId, "afr" , " 3"); 
        tato_kvlist_add(&isoToId, "ara" , " 4"); 
        tato_kvlist_add(&isoToId, "arz" , " 5"); 
        tato_kvlist_add(&isoToId, "bel" , " 6"); 
        tato_kvlist_add(&isoToId, "ben" , " 7"); 
        tato_kvlist_add(&isoToId, "bre" , " 8"); 
        tato_kvlist_add(&isoToId, "bul" , " 9"); 
        tato_kvlist_add(&isoToId, "cat" , "10"); 
        tato_kvlist_add(&isoToId, "ces" , "11"); 
        tato_kvlist_add(&isoToId, "cmn" , "12"); 
        tato_kvlist_add(&isoToId, "cycl", "13"); 
        tato_kvlist_add(&isoToId, "dan" , "14"); 
        tato_kvlist_add(&isoToId, "deu" , "15"); 
        tato_kvlist_add(&isoToId, "ell" , "16"); 
        tato_kvlist_add(&isoToId, "eng" , "17"); 
        tato_kvlist_add(&isoToId, "epo" , "18"); 
        tato_kvlist_add(&isoToId, "est" , "19"); 
        tato_kvlist_add(&isoToId, "eus" , "20"); 
        tato_kvlist_add(&isoToId, "fao" , "21"); 
        tato_kvlist_add(&isoToId, "fin" , "22"); 
        tato_kvlist_add(&isoToId, "fra" , "23"); 
        tato_kvlist_add(&isoToId, "fry" , "24"); 
        tato_kvlist_add(&isoToId, "heb" , "25"); 
        tato_kvlist_add(&isoToId, "hin" , "26"); 
        tato_kvlist_add(&isoToId, "hun" , "27"); 
        tato_kvlist_add(&isoToId, "hye" , "28"); 
        tato_kvlist_add(&isoToId, "ind" , "29"); 
        tato_kvlist_add(&isoToId, "isl" , "30"); 
        tato_kvlist_add(&isoToId, "ita" , "31"); 
        tato_kvlist_add(&isoToId, "jpn" , "32"); 
        tato_kvlist_add(&isoToId, "kat" , "33"); 
        tato_kvlist_add(&isoToId, "kaz" , "34"); 
        tato_kvlist_add(&isoToId, "kor" , "35"); 
        tato_kvlist_add(&isoToId, "lat" , "36"); 
        tato_kvlist_add(&isoToId, "lvs" , "37"); 
        tato_kvlist_add(&isoToId, "nan" , "38"); 
        tato_kvlist_add(&isoToId, "nld" , "39"); 
        tato_kvlist_add(&isoToId, "nob" , "40"); 
        tato_kvlist_add(&isoToId, "non" , "41"); 
        tato_kvlist_add(&isoToId, "pes" , "42"); 
        tato_kvlist_add(&isoToId, "pol" , "43"); 
        tato_kvlist_add(&isoToId, "por" , "44"); 
        tato_kvlist_add(&isoToId, "ron" , "45"); 
        tato_kvlist_add(&isoToId, "rus" , "46"); 
        tato_kvlist_add(&isoToId, "san" , "47"); 
        tato_kvlist_add(&isoToId, "slk" , "48"); 
        tato_kvlist_add(&isoToId, "spa" , "49"); 
        tato_kvlist_add(&isoToId, "sqi" , "50"); 
        tato_kvlist_add(&isoToId, "srp" , "51"); 
        tato_kvlist_add(&isoToId, "swe" , "52"); 
        tato_kvlist_add(&isoToId, "swh" , "53"); 
        tato_kvlist_add(&isoToId, "tat" , "54"); 
        tato_kvlist_add(&isoToId, "tur" , "55"); 
        tato_kvlist_add(&isoToId, "uig" , "56"); 
        tato_kvlist_add(&isoToId, "ukr" , "57"); 
        tato_kvlist_add(&isoToId, "urd" , "58"); 
        tato_kvlist_add(&isoToId, "uzb" , "59"); 
        tato_kvlist_add(&isoToId, "vie" , "60"); 
        tato_kvlist_add(&isoToId, "wuu" , "61"); 
        tato_kvlist_add(&isoToId, "yid" , "62"); 
        tato_kvlist_add(&isoToId, "yue" , "63"); 
        tato_kvlist_add(&isoToId, "zsm" , "64"); 
        tato_kvlist_add(&isoToId, "bos" , "65"); 
        tato_kvlist_add(&isoToId, "hrv" , "66"); 
        tato_kvlist_add(&isoToId, "orv" , "67"); 
        tato_kvlist_add(&isoToId, "cha" , "68"); 
        tato_kvlist_add(&isoToId, "tgl" , "69"); 
        tato_kvlist_add(&isoToId, "que" , "70"); 
        tato_kvlist_add(&isoToId, "mon" , "71"); 
        tato_kvlist_add(&isoToId, "lit" , "72"); 
        tato_kvlist_add(&isoToId, "glg" , "73"); 
        tato_kvlist_add(&isoToId, "gle" , "74"); 
        tato_kvlist_add(&isoToId, "ina" , "75"); 
        tato_kvlist_add(&isoToId, "jbo" , "76"); 
        tato_kvlist_add(&isoToId, "toki", "77"); 
        tato_kvlist_add(&isoToId, "ain" , "78"); 
        tato_kvlist_add(&isoToId, "scn" , "79"); 
        tato_kvlist_add(&isoToId, "mal" , "80"); 
        tato_kvlist_add(&isoToId, "nds" , "81"); 
        tato_kvlist_add(&isoToId, "tlh" , "82"); 
        tato_kvlist_add(&isoToId, "slv" , "83"); 
        tato_kvlist_add(&isoToId, "tha" , "84"); 
        tato_kvlist_add(&isoToId, "lzh" , "85"); 
        tato_kvlist_add(&isoToId, "roh" , "86"); 
        tato_kvlist_add(&isoToId, "vol" , "87");
        tato_kvlist_add(&isoToId, "oss" , "88"); 
        tato_kvlist_add(&isoToId, "ido" , "89"); 
        tato_kvlist_add(&isoToId, "gla" , "90"); 
        tato_kvlist_add(&isoToId, "ast" , "91");
    }

    return tato_kvlist_get(isoToId, code); 
}


/**
 * 
 */
void tato_item_sphinx_dump(TatoItem *thiss, FILE *f) {
	TatoRelationsNode *it = NULL;

	fprintf(
        f,
        "<sphinx:document id=\"%i\" lang=\"%s\" >\n"
        "    <tatotext><![CDATA[%s]]></tatotext>\n"
        "    <translatedin>",
		thiss->id,
		tato_item_lang_code(thiss),
		thiss->str
	);

    
    //TODO sphinx want an int not a string
	TATO_RELATIONS_FOREACH(thiss->relations, it) {
        fprintf(
            f,
            it->next ? "%s," : "%s",
            get_tato_lang_id(it->with->lang->code)
        );
    }

	fprintf(
        f,
        "%s",
        "</translatedin>\n"
    );

	if (thiss->metas != NULL) {
		tato_kvlist_sphinx_dump(thiss->metas, f);
	}
	fprintf(f, "</sphinx:document>\n"); 
}



void tato_item_debug(TatoItem *thiss) {
	printf("== TatoItem ==\n");
	printf("id: %i\n", thiss->id);
	printf("str: %s\n", thiss->str);
	printf("lang: %s\n", tato_item_lang_code(thiss));
	tato_kvlist_debug(thiss->metas);	
	tato_relations_debug(thiss->relations);	
}

