#include <sys/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <evhttp.h>

#include <tato/log.h>
/*
#include <tato/fetcher.h>
#include <tato/fetcher_tree.h>
*/
#include "output.h"
#include "http.h"

#define DEPTH_DEFAULT 0
#define DEPTH_MAX 10
#define ITEMS_SIZE_MAX 1024
#define ITEMS_SIZE_DEFAULT 8

/**
 * TODO instead of returning as soon as a mandatory param
 * is missing we should rather wait so this we will return
 * once which will make us able to free the headers at one 
 * place and also to give directly the list of mandantory
 * params which are missing
 */


#define PARSE_URI(request, params) { \
	char *uri = evhttp_decode_uri(evhttp_request_uri(request)); \
	tato_log_msg_printf(LOG_NOTICE, "GET %s", uri); \
	evhttp_parse_query(uri, &params); \
	free (uri) ;\
}

#define PARAM_GET_INT(var, params, name, mendatory) { \
	char const *arg = evhttp_find_header(params, name);  \
	if (!arg && mendatory) { \
		http_send(request, "<err message=\"field '" name "' is mendatory\"/>\n"); \
		return; \
	} \
	if (arg) {\
		var = atoi(arg); \
   }\
}

#define PARAM_GET_STR(var, params, name, mendatory) { \
	char const *arg = evhttp_find_header(&params_get, name); \
	if (!arg && mendatory) { \
		http_send(request, "<err message=\"field '" name "' is mendatory\"/>\n"); \
		return; \
	} \
	if (arg) {\
		int size = strlen(arg);\
		var = malloc(size+1);\
		strcpy(var, arg);\
		strcat(var, "\0");\
	}\
}

#define PARAM_MAX(var, value) { \
	if (var > value) var = value; \
}

#define TATO_HTTP_SEND_XML(request, buffer) { \
	evhttp_add_header((request)->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");\
	evhttp_send_reply((request), HTTP_OK, "", (buffer));\
	evbuffer_free((buffer));\
};


void http_send(struct evhttp_request *request, const char *fmt, ...) {
	struct evbuffer *buffer = evbuffer_new();
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	va_list ap;
	va_start(ap, fmt);
	evbuffer_add_vprintf(buffer, fmt, ap);
	va_end(ap);
	evhttp_send_reply(request, HTTP_OK, "", buffer);
	evbuffer_free(buffer);
}

/**** uri: *
 *
 */
void http_callback_default(struct evhttp_request *request, void *data) {
	evhttp_send_error(request, HTTP_NOTFOUND, "Service not found");
}

/**** uri: /hyperItemGet
 *
 */
void http_hyper_item_get_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_get++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperItem *item = tato_hyper_db_item_find(this->hyperdb, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_item_output_xml_tree(item, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperItemUpdate
 *
 */
void http_hyper_item_update_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_update++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperItem *item = tato_hyper_db_item_find(this->hyperdb, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "lang"
	char *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);
	if (lang) {
		tato_hyper_item_lang_set(item, tato_hyper_db_lang_find_or_create(this->hyperdb, lang));
		free(lang);
	}

	//get "str"
	char *str = NULL;
	PARAM_GET_STR(str, &params_get, "str", false);
	if (str) {
		tato_hyper_item_str_set(item, str);
		free(str);
	}


	//get "flags"
	char *flags = NULL;
	PARAM_GET_STR(flags, &params_get, "flags", false);
	if (flags) {
		tato_hyper_item_flags_set(item, atoi(flags));
		free(flags);
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	tato_hyper_item_output_xml_tree(item, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperItemRand
 *
 */
void http_hyper_item_rand_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_rand++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "lang"
	char *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);
	

	//get item
	TatoHyperItem *item = NULL;
	if (lang) {
		item = tato_hyper_db_item_rand_with_lang(this->hyperdb, lang);
		free(lang);
	} else {
		item = tato_hyper_db_item_rand(this->hyperdb);
	}

	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}
	

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_item_output_xml_tree(item, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

}

/**** uri: /hyperItemAdd
 *
 */
void http_hyper_item_add_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_add++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "lang"
	char *lang;
	PARAM_GET_STR(lang, &params_get, "lang", true);

	//get "str"
	char *str;
	PARAM_GET_STR(str, &params_get, "str", true);


	//get "flags"
	char *flags = NULL;
	PARAM_GET_STR(flags, &params_get, "flags", false);
	evhttp_clear_headers(&params_get);

	
	TatoHyperItem *item = tato_hyper_db_item_add(this->hyperdb, lang, str, flags ? atoi(flags) : 0);

	free(lang);
	free(str);
	if(flags) free(flags);

	if (!item) {
		http_send(request, "<err message=\"Could not create item\"/>\n");
		return;
	}

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_item_output_xml(item, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /hyperItemMerge
 *
 */
void http_hyper_item_merge_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_merge++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get id
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	//get into
	TatoHyperItemId into;
	PARAM_GET_INT(into, &params_get, "into", true);
	TatoHyperItem *item = tato_hyper_db_item_find(this->hyperdb, into);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	evhttp_clear_headers(&params_get);

	//merge
	if (!tato_hyper_db_item_merge_into(this->hyperdb, id, item)) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}
	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_item_output_xml(item, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperItemAddMutualTranslationRelation
 * use to create 2 links between the two items 
 * if a translation link already exist between both 
 * it will use it instead of creating a new one
 */

void http_hyper_item_add_mutal_translation_relation_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_add_mutal_translation_relation++;
    


	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "x"
	TatoHyperItemId x;
	PARAM_GET_INT(x, &params_get, "x", true);
	TatoHyperItem *xItem = tato_hyper_db_item_find(this->hyperdb, x);
	if (!xItem) {
		evhttp_clear_headers(&params_get);
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "y"
	TatoHyperItemId y;
	PARAM_GET_INT(y, &params_get, "y", true);
	TatoHyperItem *yItem = tato_hyper_db_item_find(this->hyperdb, y);
	if (!yItem) {
		evhttp_clear_headers(&params_get);
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	evhttp_clear_headers(&params_get);


    bool added = false;
    TatoHyperRelationsNode *it;
    TATO_HYPER_RELATIONS_FOREACH(xItem->startofs, it) {
        if (it->relation->type == 1) {//SHDICT_TRANSLATION_REL_FLAG
            tato_hyper_db_relation_add_end(
                this->hyperdb,
                it->relation->id,
                y
            );
            added = true;
            break;
        }

    }
    if (!added) {
        tato_hyper_db_relation_add(
            this->hyperdb,
            x,
            y,
            1, //SHDICT_TRANSLATION_REL_FLAG
            0
        );
    }

    // now the other way relation
    added = false;
    TatoHyperRelationsNode *it2;
    TATO_HYPER_RELATIONS_FOREACH(yItem->startofs, it2) {
        if (it2->relation->type == 1) {//SHDICT_TRANSLATION_REL_FLAG
            tato_hyper_db_relation_add_end(
                this->hyperdb,
                it2->relation->id,
                x
            );
            added = true;
            break;
        }

    }
    if (!added) {
        tato_hyper_db_relation_add(
            this->hyperdb,
            y,
            x,
            1, //SHDICT_TRANSLATION_REL_FLAG
            0
        );
    }


	http_send(request, "<ok/>\n");

}
/**** uri: /hyperItemMetaSet
 *
 */
void http_hyper_item_meta_set_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_meta_set++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperItem *item = tato_hyper_db_item_find(this->hyperdb, id);
	if (!item) {
		evhttp_clear_headers(&params_get);
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "key"
	char *key = NULL;
	PARAM_GET_STR(key, &params_get, "key", true);

	//get "value"
	char *value = NULL;
	PARAM_GET_STR(value, &params_get, "value", true);

	evhttp_clear_headers(&params_get);

	if (!tato_hyper_item_meta_set(item, key, value)) {
		tato_hyper_item_meta_add(
			item,
			tato_hyper_db_common_str(this->hyperdb, key),
			value
		);
	}

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_item_output_xml(item, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(key);
	free(value);
}

/**** uri: /hyperItemMetaUnset
 *
 */
void http_hyper_item_meta_unset_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_meta_unset++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperItem *item = tato_hyper_db_item_find(this->hyperdb, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "key"
	char *key = NULL;
	PARAM_GET_STR(key, &params_get, "key", true);

	evhttp_clear_headers(&params_get);
	
	tato_hyper_item_meta_delete(item, key);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_item_output_xml(item, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(key);
}

/**** uri: /hyperItemDelete
 *
 */
void http_hyper_item_delete_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_item_delete++;

	//parse uri
	struct evkeyvalq params_get;

	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	evhttp_clear_headers(&params_get);

	if (!tato_hyper_db_item_delete(this->hyperdb, id)) {
		http_send(request, "<err message=\"Could not delete item\"/>\n");
		return;
	}

	http_send(request, "<ok/>\n");
}

/**** uri: /hyperItemsRand
 *
 */
void http_hyper_items_rand_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_items_rand++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//get "size"
	int size = ITEMS_SIZE_DEFAULT;
	PARAM_GET_INT(size, &params_get, "size", false);
	PARAM_MAX(size, ITEMS_SIZE_MAX);

	//get "lang"
	char *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);
	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();


	//get items
	evbuffer_add_printf(buffer, "<hyperitems>\n");
	
	int i;
	for (i = 0; i < size; i++) {
		TatoHyperItem *item = (lang)
			? tato_hyper_db_item_rand_with_lang(this->hyperdb, lang)
			: tato_hyper_db_item_rand(this->hyperdb);

		if (!item) 
			break;

		tato_hyper_item_output_xml_tree (item, buffer, this->plugins);
	}
	evbuffer_add_printf(buffer, "</hyperitems>\n");

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperItemsGet
 *
 */
void http_hyper_items_get_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_items_get++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//get "size"
	int size = ITEMS_SIZE_DEFAULT;
	PARAM_GET_INT(size, &params_get, "size", false);
	PARAM_MAX(size, ITEMS_SIZE_MAX);

	//get "ids"
	char *ids;
	PARAM_GET_STR(ids, &params_get, "ids", true);
	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	//get items
	evbuffer_add_printf(buffer, "<hyperitems>\n");
	char *list = strdup(ids);
	char *id = strtok(list, ",");

	
	while (id) {
		TatoHyperItem *item = tato_hyper_db_item_find(this->hyperdb, atoi(id));
		if (item) {
			tato_hyper_item_output_xml_tree(item, buffer, this->plugins);
		}
		else {
			evbuffer_add_printf(buffer, "<err message=\"Item %i not found\"/>\n", atoi(id));
		}
		id = strtok(NULL, ",");
	}
	free(list);
	free(ids);
	evbuffer_add_printf(buffer, "</hyperitems>\n");

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperItemsRelated
 *
 */
void http_hyper_items_related_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_items_related++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//get "size"
	int size = ITEMS_SIZE_DEFAULT;
	PARAM_GET_INT(size, &params_get, "size", false);
	PARAM_MAX(size, ITEMS_SIZE_MAX);

	//get "offset"
	int offset = 0;
	PARAM_GET_INT(offset, &params_get, "offset", false);

	//get "inversed"
	int inversed = 0;
	PARAM_GET_INT(inversed, &params_get, "inversed", false);

	//get "lang"
	char *lang;
	PARAM_GET_STR(lang, &params_get, "lang", true);

	TatoHyperItemLang *tato_hyper_item_lang = tato_hyper_db_lang_find(this->hyperdb, lang);
	if (!tato_hyper_item_lang) {
		evhttp_clear_headers(&params_get);
		http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
		return;
	}

	//get "with"
	char *with;
	PARAM_GET_STR(with, &params_get, "with", true);

	evhttp_clear_headers(&params_get);

	TatoHyperItemLang *tato_with = NULL;
	if (strcmp(with, "none") != 0) {
		tato_with = tato_hyper_db_lang_find(this->hyperdb, with);
		if (!tato_with) {
			http_send(request, "<err message=\"'with' does'nt exists\"/>\n");
			return;
		}
	}


	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	
	TatoHyperItemFetcher *fetcher = tato_hyper_item_fetcher_new(size, offset);
	tato_hyper_item_lang_items_related_with(tato_hyper_item_lang, fetcher, tato_with, inversed);
	tato_hyper_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_hyper_item_fetcher_free(fetcher);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(lang);
	free(with);
}

/**** uri: /hyperItemsSearch
 *
 */
void http_hyper_items_search_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_items_search++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//get "size"
	int size = ITEMS_SIZE_DEFAULT;
	PARAM_GET_INT(size, &params_get, "size", false);
	PARAM_MAX(size, ITEMS_SIZE_MAX);

	//get "offset"
	int offset = 0;
	PARAM_GET_INT(offset, &params_get, "offset", false);

	//get "str"
	char *str = NULL;
	PARAM_GET_STR(str, &params_get, "str", true);

	//get "lang"
	char *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);

	evhttp_clear_headers(&params_get);

	TatoHyperItemLang *tato_hyper_item_lang = NULL;
	if (lang) {
		tato_hyper_item_lang = tato_hyper_db_lang_find(this->hyperdb, lang);
		if (!tato_hyper_item_lang) {
			http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
			return;
		}
	}

	struct evbuffer *buffer = evbuffer_new();

	TatoHyperItemFetcher *fetcher = tato_hyper_item_fetcher_new(size, offset);
	if (tato_hyper_item_lang) {
		tato_hyper_item_lang_items_search(tato_hyper_item_lang, fetcher, str);
	}
	else {
		tato_hyper_db_items_search(this->hyperdb, fetcher, str);
	}

	tato_hyper_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_hyper_item_fetcher_free(fetcher);


	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(lang);
	free(str);
}



/**** uri: /hyperItemsGetSeq
 * retrieve the Xth Item and the Y followings
 */
void http_hyper_items_get_seq_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_items_search++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//get "size"
	int size = ITEMS_SIZE_DEFAULT;
	PARAM_GET_INT(size, &params_get, "size", false);
	PARAM_MAX(size, ITEMS_SIZE_MAX);

	//get "offset"
	int offset = 0;
	PARAM_GET_INT(offset, &params_get, "offset", false);

	//get "lang"
	char *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);

	evhttp_clear_headers(&params_get);

	TatoHyperItemLang *tato_hyper_item_lang = NULL;
	if (lang) {
		tato_hyper_item_lang = tato_hyper_db_lang_find(this->hyperdb, lang);
		if (!tato_hyper_item_lang) {
			http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
			return;
		}
	}

	struct evbuffer *buffer = evbuffer_new();

	TatoHyperItemFetcher *fetcher = tato_hyper_item_fetcher_new(size, offset);
	if (tato_hyper_item_lang) {
		tato_hyper_item_lang_items_get_seq(tato_hyper_item_lang, fetcher);
	}
	else {
		tato_hyper_db_items_get_seq(this->hyperdb, fetcher);
	}

	tato_hyper_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_hyper_item_fetcher_free(fetcher);


	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(lang);
}




/**** uri: /hyperItemsSuggest
 *
 */
void http_hyper_items_suggest_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_items_suggest++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//get "size"
	int size = ITEMS_SIZE_DEFAULT;
	PARAM_GET_INT(size, &params_get, "size", false);
	PARAM_MAX(size, ITEMS_SIZE_MAX);

	//get "offset"
	int offset = 0;
	PARAM_GET_INT(offset, &params_get, "offset", false);

	//get "str"
	char *str = NULL;
	PARAM_GET_STR(str, &params_get, "str", true);

	//get "lang"
	char *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", true);

	evhttp_clear_headers(&params_get);

	TatoHyperItemLang *tato_hyper_item_lang = tato_hyper_db_lang_find(this->hyperdb, lang);
	if (!tato_hyper_item_lang) {
		http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
		return;
	}

	struct evbuffer *buffer = evbuffer_new();
	
	TatoHyperItemFetcher *fetcher = tato_hyper_item_fetcher_new(size, offset);
	tato_hyper_item_lang_items_suggest(tato_hyper_item_lang, fetcher, str);
	tato_hyper_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_hyper_item_fetcher_free(fetcher);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(str);
	free(lang);
}

/**** uri: /hyperRelationGet
 *
 */
void http_hyper_relation_get_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_get++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperRelation *relation = tato_hyper_db_relation_find(this->hyperdb, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		evhttp_clear_headers(&params_get);
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	//tato_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);
	tato_hyper_relation_output_xml_tree(relation, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperRelationUpdate
 *
 */
void http_hyper_relation_update_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_update++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperRelation *relation = tato_hyper_db_relation_find(this->hyperdb, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		return;
	}

	//get "type"
	char *type = NULL;
	PARAM_GET_STR(type, &params_get, "type", false);
	if (type) {
		tato_hyper_relation_type_set(relation, atoi(type));
		free(type);
	}

	//get "flags"
	char *flags = NULL;
	PARAM_GET_STR(flags, &params_get, "flags", false);
	if (flags) {
		tato_hyper_relation_flags_set(relation, atoi(flags));
		free(flags);
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);
	
	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

/**** uri: /hyperRelationAdd
 *
 */
void http_hyper_relation_add_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_add++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "start"
	TatoHyperItemId start;
	PARAM_GET_INT(start, &params_get, "start", true);

	//get "end"
	TatoHyperItemId end;
	PARAM_GET_INT(end, &params_get, "end", true);

	//get "type"
	TatoHyperRelationType type = 0;
	PARAM_GET_INT(type, &params_get, "type", false);

	//get "flags"
	TatoHyperRelationFlags flags = 0;
	PARAM_GET_INT(flags, &params_get, "flags", false);

	evhttp_clear_headers(&params_get);

	TatoHyperRelation *relation = tato_hyper_db_relation_add(
		this->hyperdb,
		start,
		end,
		type,
		flags
	);

	if (!relation) {
		http_send(request, "<err message=\"Could not create relation\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);
	
	evhttp_clear_headers(&params_get);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}


/**** uri: /hyperRelationAddEnd
 *
 */
void http_hyper_relation_add_end_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_add++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "relation"
	TatoHyperRelationId relationid;
	PARAM_GET_INT(relationid, &params_get, "relation", true);

	//get "end"
	TatoHyperItemId end;
	PARAM_GET_INT(end, &params_get, "end", true);

	evhttp_clear_headers(&params_get);

	TatoHyperRelation *relation = tato_hyper_db_relation_add_end(
		this->hyperdb,
		relationid,
		end
	);

	if (!relation) {
		http_send(request, "<err message=\"Could not create relation\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	evhttp_clear_headers(&params_get);

	//prepare output
	
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}




/**** uri: /hyperRelationDelete
 *
 */
void http_hyper_relation_delete_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_delete++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	evhttp_clear_headers(&params_get);

	if (!tato_hyper_db_relation_delete(this->hyperdb, id)) {
		http_send(request, "<err message=\"Could not delete relation\"/>\n");
		return;
	}

	http_send(request, "<ok/>\n");
}



/**** uri: /hyperRelationDeleteEnd
 *
 */
void http_hyper_relation_delete_end_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_delete++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	//get "end"
	TatoHyperItemId end;
	PARAM_GET_INT(end, &params_get, "end", true);

	evhttp_clear_headers(&params_get);

	TatoHyperRelation *relation = tato_hyper_db_relation_remove_end(this->hyperdb, id, end);
	// TODO for the moment we don't check if the relation didn't exist
	// if the item was or not part of the end, if everything goes well ...
	// we only know if a the end the relation exist or not
	if (!relation) {
		http_send(request, "<ok/>\n");
		return;
	}
	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	//tato_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);
	tato_hyper_relation_output_xml_tree(relation, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}




/**** uri: /hyperRelationMetaSet
 *
 */
void http_hyper_relation_meta_set_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.hyper_relation_meta_set++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperRelation *relation = tato_hyper_db_relation_find(this->hyperdb, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		evhttp_clear_headers(&params_get);
		return;
	}

	//get "key"
	char *key = NULL;
	PARAM_GET_STR(key, &params_get, "key", true);

	//get "value"
	char *value = NULL;
	PARAM_GET_STR(value, &params_get, "value", true);

	evhttp_clear_headers(&params_get);

	if (!tato_hyper_relation_meta_set(relation, key, value)) {
		tato_hyper_relation_meta_add(
			relation,
			tato_hyper_db_common_str(this->hyperdb, key),
			value
		);
	}

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_relation_output_xml(relation, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(key);
	free(value);
}

/**** uri: /hyperRelationMetaUnset
 *
 */
void http_hyper_relation_meta_unset_callback(struct evhttp_request *request, void *data) {

	Http *this = (Http *) data;
	this->stats.hyper_relation_meta_unset++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoHyperRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoHyperRelation *relation = tato_hyper_db_relation_find(this->hyperdb, id);
	if (!relation) {
		evhttp_clear_headers(&params_get);
		http_send(request, "<err message=\"Relation not found\"/>\n");
		return;
	}

	//get "key"
	char *key = NULL;
	PARAM_GET_STR(key, &params_get, "key", true);

	evhttp_clear_headers(&params_get);

	tato_hyper_relation_meta_delete(relation, key);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_hyper_relation_output_xml(relation, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)

	free(key);
}

/**** uri: /dump
 *
 */
void http_dump_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.dump++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "path"
	char *path = NULL;
	PARAM_GET_STR(path, &params_get, "path", false);
	
	evhttp_clear_headers(&params_get);

	if (!path) path = this->hyperdb->default_path;

	tato_hyper_db_debug(this->hyperdb);
	if (tato_hyper_db_dump_forked(this->hyperdb, path)) {
		http_send(request, "<ok/>\n");
	} else {
		http_send(request, "<err message=\"Problem while tenting to fork\" />\n");
	}
}

/**** uri: /stats 
 *
 */
#define HOSTNAME_BUFFER_SIZE 128
#define EVBUFFER_ADD_STATS(buffer, name, field) evbuffer_add_printf(buffer, name " %i\n", this->stats.field);
void http_stats_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.stats++;

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	evbuffer_add_printf(buffer, "[SERVER]\n");

	//hostname 
	char hostname_buffer[HOSTNAME_BUFFER_SIZE];
	if (gethostname(hostname_buffer, HOSTNAME_BUFFER_SIZE) == 0) {
		hostname_buffer[HOSTNAME_BUFFER_SIZE - 1] = '\0';
		evbuffer_add_printf(buffer, "hostname %s\n", hostname_buffer);
	}

	evbuffer_add_printf(buffer, "version %s\n", VERSION);
	evbuffer_add_printf(buffer, "uptime %li\n", time(NULL) - this->started_at);

	evbuffer_add_printf(buffer, "\n[QUERIES]\n");
	EVBUFFER_ADD_STATS(buffer, "hyperItemGet", hyper_item_get);
	EVBUFFER_ADD_STATS(buffer, "hyperItemRand", hyper_item_rand);
	EVBUFFER_ADD_STATS(buffer, "hyperItemAdd", hyper_item_add);
	EVBUFFER_ADD_STATS(buffer, "hyperItemUpdate", hyper_item_update);
	EVBUFFER_ADD_STATS(buffer, "hyperItemDelete", hyper_item_delete);
	EVBUFFER_ADD_STATS(buffer, "hyperItemMerge", hyper_item_merge);
	EVBUFFER_ADD_STATS(buffer, "hyperItemMetaSet", hyper_item_meta_set);
	EVBUFFER_ADD_STATS(buffer, "hyperItemMetaUnset", hyper_item_meta_unset);
	EVBUFFER_ADD_STATS(buffer, "hyperItemsGet", hyper_items_get);
	EVBUFFER_ADD_STATS(buffer, "hyperItemsRelated", hyper_items_related);
	EVBUFFER_ADD_STATS(buffer, "hyperItemsRand", hyper_items_rand);
	EVBUFFER_ADD_STATS(buffer, "hyperItemsSearch", hyper_items_search);
	EVBUFFER_ADD_STATS(buffer, "hyperItemsSuggest", hyper_items_suggest);
	EVBUFFER_ADD_STATS(buffer, "hyperRelationGet", hyper_relation_get);
	EVBUFFER_ADD_STATS(buffer, "hyperRelationAdd", hyper_relation_add);
	EVBUFFER_ADD_STATS(buffer, "hyperRelationUpdate", hyper_relation_update);
	EVBUFFER_ADD_STATS(buffer, "hyperRelationDelete", hyper_relation_delete);
	EVBUFFER_ADD_STATS(buffer, "hyperRelationMetaSet", hyper_relation_meta_set);
	EVBUFFER_ADD_STATS(buffer, "hyperRelationMetaUnset", hyper_relation_meta_unset);
	EVBUFFER_ADD_STATS(buffer, "hyperItemAddMutualTranslationRelation", hyper_item_add_mutal_translation_relation);
	EVBUFFER_ADD_STATS(buffer, "dump", dump);
	EVBUFFER_ADD_STATS(buffer, "stats", stats);

	//stats de la hyperdb
	tato_hyper_db_output_stats(this->hyperdb, buffer);
	plugins_output_stats(this->plugins, buffer);

	//send
	TATO_HTTP_SEND_XML(request, buffer)
}

// --------------------

Http *http_new(TatoHyperDb *hyperdb, Plugins *plugins, Args *args) {
	Http *this = malloc(sizeof(Http));
	this->started_at = 0;
	this->hyperdb = hyperdb;
	this->plugins = plugins;
	this->args = args;
	this->server = NULL;
	this->base = NULL;
	http_clear_stats(this);
	return this;
}

void http_free(Http *this) {
	printf("We're in free http \n");

	tato_hyper_db_free(this->hyperdb);
	args_free(this->args);
	plugins_free(this->plugins);

	if (this->server != NULL) {
		evhttp_del_cb(this->server, "/hyperItemGet");
		evhttp_del_cb(this->server, "/hyperItemUpdate");
		evhttp_del_cb(this->server, "/hyperItemRand");
		evhttp_del_cb(this->server, "/hyperItemAdd");
		evhttp_del_cb(this->server, "/hyperItemDelete");
		evhttp_del_cb(this->server, "/hyperItemMerge");
		evhttp_del_cb(this->server, "/hyperItemMetaSet");
		evhttp_del_cb(this->server, "/hyperItemMetaUnset");
		evhttp_del_cb(this->server, "/hyperItemsGet");
		evhttp_del_cb(this->server, "/hyperItemsRand");
		evhttp_del_cb(this->server, "/hyperItemsRelated");
		evhttp_del_cb(this->server, "/hyperItemsSearch");
		evhttp_del_cb(this->server, "/hyperItemsSuggest");
		evhttp_del_cb(this->server, "/hyperRelationGet");
		evhttp_del_cb(this->server, "/hyperRelationUpdate");
		evhttp_del_cb(this->server, "/hyperRelationAdd");
		evhttp_del_cb(this->server, "/hyperRelationAddEnd");
		evhttp_del_cb(this->server, "/hyperRelationDelete");
		evhttp_del_cb(this->server, "/hyperRelationDeleteEnd");
		evhttp_del_cb(this->server, "/hyperRelationMetaSet");
		evhttp_del_cb(this->server, "/hyperRelationMetaUnset");

        evhttp_del_cb(this->server, "/hyperItemAddMutualTranslationRelation");

		evhttp_del_cb(this->server, "/dump");
		evhttp_del_cb(this->server, "/stats");

		evhttp_free(this->server);
	}

	if (this->base != NULL) {
		event_base_free(this->base);
	}


	free(this);
}

void http_clear_stats(Http *this) {
	this->stats.hyper_item_get = 0;
	this->stats.hyper_item_rand = 0;
	this->stats.hyper_item_add = 0;
	this->stats.hyper_item_update = 0;
	this->stats.hyper_item_delete = 0;
	this->stats.hyper_item_merge = 0;
	this->stats.hyper_item_meta_set = 0;
	this->stats.hyper_item_meta_unset = 0;
	this->stats.hyper_items_get = 0;
	this->stats.hyper_items_rand = 0;
	this->stats.hyper_items_related = 0;
	this->stats.hyper_items_search = 0;
	this->stats.hyper_items_suggest = 0;
	this->stats.hyper_relation_get = 0;
	this->stats.hyper_relation_add = 0;
	this->stats.hyper_relation_update = 0;
	this->stats.hyper_relation_delete = 0;
	this->stats.hyper_relation_meta_set = 0;
	this->stats.hyper_relation_meta_unset = 0;

    this->stats.hyper_item_add_mutal_translation_relation = 0;

	this->stats.dump = 0;
	this->stats.stats = 0;
}

void http_start(Http *this, char const *addr, int port) {
	tato_log_msg_printf(LOG_NOTICE, "Start HTTP server (%s:%i)", addr, port);

	this->started_at = time(NULL);

	this->base = event_init();
	this->server = evhttp_new(this->base);
	int res = evhttp_bind_socket(this->server, addr, port);
	if (res != 0) {
		tato_log_msg_printf(LOG_ERR, "Could not start http server!");
		return;
	}

	evhttp_set_gencb(this->server, http_callback_default, this);
	evhttp_set_cb(this->server, "/hyperItemGet", http_hyper_item_get_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperItemUpdate", http_hyper_item_update_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperItemRand", http_hyper_item_rand_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperItemAdd", http_hyper_item_add_callback, this);//OK
	evhttp_set_cb(this->server, "/hyperItemDelete", http_hyper_item_delete_callback, this);//OK
	evhttp_set_cb(this->server, "/hyperItemMerge", http_hyper_item_merge_callback, this);//OK
	evhttp_set_cb(this->server, "/hyperItemMetaSet", http_hyper_item_meta_set_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperItemMetaUnset", http_hyper_item_meta_unset_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperItemsGet", http_hyper_items_get_callback, this); // OK
	evhttp_set_cb(this->server, "/hyperItemsRand", http_hyper_items_rand_callback, this); // OK
	evhttp_set_cb(this->server, "/hyperItemsRelated", http_hyper_items_related_callback, this);// OK
	evhttp_set_cb(this->server, "/hyperItemsGetSeq", http_hyper_items_get_seq_callback, this);
	evhttp_set_cb(this->server, "/hyperItemsSearch", http_hyper_items_search_callback, this); // OK
	evhttp_set_cb(this->server, "/hyperItemsSuggest", http_hyper_items_suggest_callback, this);// OK
	evhttp_set_cb(this->server, "/hyperRelationGet", http_hyper_relation_get_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperRelationUpdate", http_hyper_relation_update_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperRelationAdd", http_hyper_relation_add_callback, this);//OK
	evhttp_set_cb(this->server, "/hyperRelationAddEnd", http_hyper_relation_add_end_callback, this);//OK
	evhttp_set_cb(this->server, "/hyperRelationDelete", http_hyper_relation_delete_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperRelationDeleteEnd", http_hyper_relation_delete_end_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperRelationMetaSet", http_hyper_relation_meta_set_callback, this); //OK
	evhttp_set_cb(this->server, "/hyperRelationMetaUnset", http_hyper_relation_meta_unset_callback, this);//OK


	evhttp_set_cb(this->server, "/hyperItemAddMutualTranslationRelation", http_hyper_item_add_mutal_translation_relation_callback, this);//OK

    
	evhttp_set_cb(this->server, "/dump", http_dump_callback, this); // OK
	evhttp_set_cb(this->server, "/stats", http_stats_callback, this); // OK

	event_base_dispatch(this->base);
}
