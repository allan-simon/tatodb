#include <sys/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <evhttp.h>

#include <tato/log.h>
#include <tato/fetcher.h>
#include <tato/fetcher_tree.h>

#include "output.h"
#include "http.h"

#define DEPTH_DEFAULT 0
#define DEPTH_MAX 10
#define ITEMS_SIZE_MAX 1024
#define ITEMS_SIZE_DEFAULT 8

#define PARSE_URI(request, params) { \
	char const *uri = evhttp_request_uri(request); \
	tato_log_msg_printf(LOG_NOTICE, "GET %s", uri); \
	evhttp_parse_query(uri, &params); \
}

#define PARAM_GET_INT(var, params, name, mendatory) { \
	char const *arg = evhttp_find_header(params, name);  \
	if (!arg && mendatory) { \
		http_send(request, "<err message=\"field '" name "' is mendatory\"/>\n"); \
		return; \
	} \
	if (arg) var = atoi(arg); \
}

#define PARAM_GET_STR(var, params, name, mendatory) { \
	var = evhttp_find_header(&params_get, name); \
	if (!var && mendatory) { \
		http_send(request, "<err message=\"field '" name "' is mendatory\"/>\n"); \
		return; \
	} \
}

#define PARAM_MAX(var, value) { \
	if (var > value) var = value; \
}

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

/**** uri: /itemGet
 *
 */
void http_item_get_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_get++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoItem *item = tato_db_item_find(this->db, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	TatoFetcherTree *tree = tato_fetcher_tree_new();
	tato_fetcher_tree_bfs(tree, item, depth, NULL);
	tato_fetcher_tree_output_xml(tree, buffer, this->plugins);
	tato_fetcher_tree_free(tree);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemUpdate
 *
 */
void http_item_update_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_update++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoItem *item = tato_db_item_find(this->db, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "lang"
	char const *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);
	if (lang) {
		tato_item_lang_set(item, tato_db_lang_find_or_create(this->db, lang));
	}

	//get "str"
	char const *str = NULL;
	PARAM_GET_STR(str, &params_get, "str", false);
	if (str) {
		tato_item_str_set(item, str);
	}

	//get "flags"
	char const *flags = NULL;
	PARAM_GET_STR(flags, &params_get, "flags", false);
	if (flags) {
		tato_item_flags_set(item, atoi(flags));
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	TatoFetcherTree *tree = tato_fetcher_tree_new();
	tato_fetcher_tree_bfs(tree, item, depth, NULL);
	tato_fetcher_tree_output_xml(tree, buffer, this->plugins);
	tato_fetcher_tree_free(tree);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemRand
 *
 */
void http_item_rand_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_rand++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "lang"
	char const *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);

	//get item
	TatoItem *item = (lang) ? tato_db_item_rand_with_lang(this->db, lang) : tato_db_item_rand(this->db);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	TatoFetcherTree *tree = tato_fetcher_tree_new();
	tato_fetcher_tree_bfs(tree, item, depth, NULL);
	tato_fetcher_tree_output_xml(tree, buffer, this->plugins);
	tato_fetcher_tree_free(tree);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemAdd
 *
 */
void http_item_add_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_add++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "lang"
	char const *lang;
	PARAM_GET_STR(lang, &params_get, "lang", true);

	//get "str"
	char const *str;
	PARAM_GET_STR(str, &params_get, "str", true);

	//get "flags"
	char const *flags = NULL;
	PARAM_GET_STR(flags, &params_get, "flags", false);
	
	TatoItem *item = tato_db_item_add(this->db, lang, str, flags ? atoi(flags) : 0);
	if (!item) {
		http_send(request, "<err message=\"Could not create item\"/>\n");
		return;
	}

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_item_output_xml(item, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemMerge
 *
 */
void http_item_merge_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_merge++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get id
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	//get into
	TatoItemId into;
	PARAM_GET_INT(into, &params_get, "into", true);
	TatoItem *item = tato_db_item_find(this->db, into);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//merge
	if (!tato_db_item_merge_into(this->db, id, item)) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	TatoFetcherTree *tree = tato_fetcher_tree_new();
	tato_fetcher_tree_bfs(tree, item, depth, NULL);
	tato_fetcher_tree_output_xml(tree, buffer, this->plugins);
	tato_fetcher_tree_free(tree);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemMetaSet
 *
 */
void http_item_meta_set_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_meta_set++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoItem *item = tato_db_item_find(this->db, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "key"
	char const *key;
	PARAM_GET_STR(key, &params_get, "key", true);

	//get "value"
	char const *value;
	PARAM_GET_STR(value, &params_get, "value", true);

	if (!tato_item_meta_set(item, key, value))
		tato_item_meta_add(item, tato_db_common_str(this->db, key), value);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_item_output_xml(item, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemMetaUnset
 *
 */
void http_item_meta_unset_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_meta_unset++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoItem *item = tato_db_item_find(this->db, id);
	if (!item) {
		http_send(request, "<err message=\"Item not found\"/>\n");
		return;
	}

	//get "key"
	char const *key;
	PARAM_GET_STR(key, &params_get, "key", true);

	tato_item_meta_delete(item, key);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_item_output_xml(item, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemDelete
 *
 */
void http_item_delete_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.item_delete++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	if (!tato_db_item_delete(this->db, id)) {
		http_send(request, "<err message=\"Could not delete item\"/>\n");
		return;
	}

	http_send(request, "<ok/>\n");
}

/**** uri: /itemsRand
 *
 */
void http_items_rand_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.items_rand++;

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
	char const *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	//get items
	evbuffer_add_printf(buffer, "<items>\n");
	TatoFetcherTree *tree = tato_fetcher_tree_new();
	int i;
	for (i = 0; i < size; i++) {
		TatoItem *item = (lang) ? tato_db_item_rand_with_lang(this->db, lang) : tato_db_item_rand(this->db);
		if (!item) 
			break;

		tato_fetcher_tree_bfs(tree, item, depth, NULL);
		tato_fetcher_tree_output_xml(tree, buffer, this->plugins);
		tato_fetcher_tree_clear(tree);
	}
	tato_fetcher_tree_free(tree);
	evbuffer_add_printf(buffer, "</items>\n");

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemsGet
 *
 */
void http_items_get_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.items_get++;

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
	char const *ids;
	PARAM_GET_STR(ids, &params_get, "ids", true);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();

	//get items
	evbuffer_add_printf(buffer, "<items>\n");
	char *list = strdup(ids);
	char *id = strtok(list, ",");
	TatoFetcherTree *tree = tato_fetcher_tree_new();
	while (id) {
		TatoItem *item = tato_db_item_find(this->db, atoi(id));
		if (item) {
			tato_fetcher_tree_bfs(tree, item, depth, NULL);
			tato_fetcher_tree_output_xml(tree, buffer, this->plugins);
			tato_fetcher_tree_clear(tree);
		}
		else {
			evbuffer_add_printf(buffer, "<err message=\"Item %i not found\"/>\n", atoi(id));
		}
		id = strtok(NULL, ",");
	}
	tato_fetcher_tree_free(tree);
	free(list);
	evbuffer_add_printf(buffer, "</items>\n");

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemsRelated
 *
 */
void http_items_related_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.items_related++;

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

	//get "offset"
	int inversed = 0;
	PARAM_GET_INT(inversed, &params_get, "inversed", false);

	//get "lang"
	char const *lang;
	PARAM_GET_STR(lang, &params_get, "lang", true);

	TatoItemLang *tato_item_lang = tato_db_lang_find(this->db, lang);
	if (!tato_item_lang) {
		http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
		return;
	}

	//get "with"
	char const *with;
	PARAM_GET_STR(with, &params_get, "with", true);

	TatoItemLang *tato_with = NULL;
	if (strcmp(with, "none") != 0) {
		tato_with = tato_db_lang_find(this->db, with);
		if (!tato_with) {
			http_send(request, "<err message=\"'with' does'nt exists\"/>\n");
			return;
		}
	}

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	TatoItemFetcher *fetcher = tato_item_fetcher_new(size, offset);
	tato_item_lang_items_related_with(tato_item_lang, fetcher, tato_with, inversed);
	tato_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_item_fetcher_free(fetcher);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemsSearch
 *
 */
void http_items_search_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.items_search++;

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
	char const *str;
	PARAM_GET_STR(str, &params_get, "str", true);

	//get "lang"
	char const *lang = NULL;
	PARAM_GET_STR(lang, &params_get, "lang", false);

	TatoItemLang *tato_item_lang = NULL;
	if (lang) {
		tato_item_lang = tato_db_lang_find(this->db, lang);
		if (!tato_item_lang) {
			http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
			return;
		}
	}


	TatoItemFetcher *fetcher = tato_item_fetcher_new(size, offset);
	if (tato_item_lang) {
		tato_item_lang_items_search(tato_item_lang, fetcher, str);
	}
	else {
		tato_db_items_search(this->db, fetcher, str);
	}

	struct evbuffer *buffer = evbuffer_new();
	tato_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_item_fetcher_free(fetcher);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /itemsSuggest
 *
 */
void http_items_suggest_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.items_suggest++;

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
	char const *str;
	PARAM_GET_STR(str, &params_get, "str", true);

	//get "lang"
	char const *lang;
	PARAM_GET_STR(lang, &params_get, "lang", true);

	TatoItemLang *tato_item_lang = tato_db_lang_find(this->db, lang);
	if (!tato_item_lang) {
		http_send(request, "<err message=\"'lang' does'nt exists\"/>\n");
		return;
	}

	struct evbuffer *buffer = evbuffer_new();
	TatoItemFetcher *fetcher = tato_item_fetcher_new(size, offset);
	tato_item_lang_items_suggest(tato_item_lang, fetcher, str);
	tato_item_fetcher_output_xml(fetcher, buffer, depth, NULL, this->plugins);
	tato_item_fetcher_free(fetcher);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /relationGet
 *
 */
void http_relation_get_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.relation_get++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoRelation *relation = tato_db_relation_find(this->db, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /relationUpdate
 *
 */
void http_relation_update_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.relation_update++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoItemId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoRelation *relation = tato_db_relation_find(this->db, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		return;
	}

	//get "type"
	char const *type = NULL;
	PARAM_GET_STR(type, &params_get, "type", false);
	if (type) {
		tato_relation_type_set(relation, atoi(type));
	}

	//get "flags"
	char const *flags = NULL;
	PARAM_GET_STR(flags, &params_get, "flags", false);
	if (flags) {
		tato_relation_flags_set(relation, atoi(flags));
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /relationAdd
 *
 */
void http_relation_add_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.relation_add++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "x"
	TatoItemId x;
	PARAM_GET_INT(x, &params_get, "x", true);

	//get "y"
	TatoItemId y;
	PARAM_GET_INT(y, &params_get, "y", true);

	//get "type"
	TatoRelationType type = 0;
	PARAM_GET_INT(type, &params_get, "type", false);

	//get "flags"
	TatoRelationFlags flags = 0;
	PARAM_GET_INT(flags, &params_get, "flags", false);

	TatoRelation *relation = tato_db_relation_add(this->db, x, y, type, flags);
	if (!relation) {
		http_send(request, "<err message=\"Could not create relation\"/>\n");
		return;
	}

	//get "depth"
	int depth = DEPTH_DEFAULT;
	PARAM_GET_INT(depth, &params_get, "depth", false);
	PARAM_MAX(depth, DEPTH_MAX);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_relation_output_xml_bfs(relation, buffer, depth, NULL, this->plugins);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /relationDelete
 *
 */
void http_relation_delete_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.relation_delete++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);

	if (!tato_db_relation_delete(this->db, id)) {
		http_send(request, "<err message=\"Could not delete relation\"/>\n");
		return;
	}

	http_send(request, "<ok/>\n");
}

/**** uri: /relationMetaSet
 *
 */
void http_relation_meta_set_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.relation_meta_set++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoRelation *relation = tato_db_relation_find(this->db, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		return;
	}

	//get "key"
	char const *key;
	PARAM_GET_STR(key, &params_get, "key", true);

	//get "value"
	char const *value;
	PARAM_GET_STR(value, &params_get, "value", true);

	if (!tato_relation_meta_set(relation, key, value))
		tato_relation_meta_add(relation, tato_db_common_str(this->db, key), value);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_relation_output_xml(relation, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

/**** uri: /relationMetaUnset
 *
 */
void http_relation_meta_unset_callback(struct evhttp_request *request, void *data) {
	Http *this = (Http *) data;
	this->stats.relation_meta_unset++;

	//parse uri
	struct evkeyvalq params_get;
	PARSE_URI(request, params_get);

	//get "id"
	TatoRelationId id;
	PARAM_GET_INT(id, &params_get, "id", true);
	TatoRelation *relation = tato_db_relation_find(this->db, id);
	if (!relation) {
		http_send(request, "<err message=\"Relation not found\"/>\n");
		return;
	}

	//get "key"
	char const *key;
	PARAM_GET_STR(key, &params_get, "key", true);

	tato_relation_meta_delete(relation, key);

	//prepare output
	struct evbuffer *buffer = evbuffer_new();
	tato_relation_output_xml(relation, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT/XML; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
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
	char const *path;
	PARAM_GET_STR(path, &params_get, "path", false);
	if (!path) path = this->db->default_path;
	if (!path) {
		http_send(request, "<err message=\"You must specify 'path'\"/>\n");
		return;
	}

	tato_db_dump_forked(this->db, path);
	http_send(request, "<ok/>\n");
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
	EVBUFFER_ADD_STATS(buffer, "itemGet", item_get);
	EVBUFFER_ADD_STATS(buffer, "itemRand", item_rand);
	EVBUFFER_ADD_STATS(buffer, "itemAdd", item_add);
	EVBUFFER_ADD_STATS(buffer, "itemUpdate", item_update);
	EVBUFFER_ADD_STATS(buffer, "itemDelete", item_delete);
	EVBUFFER_ADD_STATS(buffer, "itemMerge", item_merge);
	EVBUFFER_ADD_STATS(buffer, "itemMetaSet", item_meta_set);
	EVBUFFER_ADD_STATS(buffer, "itemMetaUnset", item_meta_unset);
	EVBUFFER_ADD_STATS(buffer, "itemsGet", items_get);
	EVBUFFER_ADD_STATS(buffer, "itemsRelated", items_related);
	EVBUFFER_ADD_STATS(buffer, "itemsRand", items_rand);
	EVBUFFER_ADD_STATS(buffer, "itemsSearch", items_search);
	EVBUFFER_ADD_STATS(buffer, "itemsSuggest", items_suggest);
	EVBUFFER_ADD_STATS(buffer, "relationGet", relation_get);
	EVBUFFER_ADD_STATS(buffer, "relationAdd", relation_add);
	EVBUFFER_ADD_STATS(buffer, "relationUpdate", relation_update);
	EVBUFFER_ADD_STATS(buffer, "relationDelete", relation_delete);
	EVBUFFER_ADD_STATS(buffer, "relationMetaSet", relation_meta_set);
	EVBUFFER_ADD_STATS(buffer, "relationMetaUnset", relation_meta_unset);
	EVBUFFER_ADD_STATS(buffer, "dump", dump);
	EVBUFFER_ADD_STATS(buffer, "stats", stats);

	//stats de la db
	tato_db_output_stats(this->db, buffer);
	plugins_output_stats(this->plugins, buffer);

	//send
	evhttp_add_header(request->output_headers, "Content-Type", "TEXT; charset=UTF8");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	evbuffer_free(buffer);
}

// --------------------

Http *http_new(TatoDb *db, Plugins *plugins) {
	Http *this = malloc(sizeof(Http));
	this->started_at = 0;
	this->db = db;
	this->plugins = plugins;
	http_clear_stats(this);
	return this;
}

void http_free(Http *this) {
	free(this);
}

void http_clear_stats(Http *this) {
	this->stats.item_get = 0;
	this->stats.item_rand = 0;
	this->stats.item_add = 0;
	this->stats.item_update = 0;
	this->stats.item_delete = 0;
	this->stats.item_merge = 0;
	this->stats.item_meta_set = 0;
	this->stats.item_meta_unset = 0;
	this->stats.items_get = 0;
	this->stats.items_rand = 0;
	this->stats.items_related = 0;
	this->stats.items_search = 0;
	this->stats.items_suggest = 0;
	this->stats.relation_get = 0;
	this->stats.relation_add = 0;
	this->stats.relation_update = 0;
	this->stats.relation_delete = 0;
	this->stats.relation_meta_set = 0;
	this->stats.relation_meta_unset = 0;
	this->stats.dump = 0;
	this->stats.stats = 0;
}

void http_start(Http *this, char const *addr, int port) {
	tato_log_msg_printf(LOG_NOTICE, "Start HTTP server (%s:%i)", addr, port);

	this->started_at = time(NULL);

	struct event_base *base = event_init();
	struct evhttp *server = evhttp_new(base);
	int res = evhttp_bind_socket(server, addr, port);
	if (res != 0) {
		tato_log_msg_printf(LOG_ERR, "Could not start http server!");
		return;
	}

	evhttp_set_gencb(server, http_callback_default, this);
	evhttp_set_cb(server, "/itemGet", http_item_get_callback, this);
	evhttp_set_cb(server, "/itemUpdate", http_item_update_callback, this);
	evhttp_set_cb(server, "/itemRand", http_item_rand_callback, this);
	evhttp_set_cb(server, "/itemAdd", http_item_add_callback, this);
	evhttp_set_cb(server, "/itemDelete", http_item_delete_callback, this);
	evhttp_set_cb(server, "/itemMerge", http_item_merge_callback, this);
	evhttp_set_cb(server, "/itemMetaSet", http_item_meta_set_callback, this);
	evhttp_set_cb(server, "/itemMetaUnset", http_item_meta_unset_callback, this);
	evhttp_set_cb(server, "/itemsGet", http_items_get_callback, this);
	evhttp_set_cb(server, "/itemsRand", http_items_rand_callback, this);
	evhttp_set_cb(server, "/itemsRelated", http_items_related_callback, this);
	evhttp_set_cb(server, "/itemsSearch", http_items_search_callback, this);
	evhttp_set_cb(server, "/itemsSuggest", http_items_suggest_callback, this);
	evhttp_set_cb(server, "/relationGet", http_relation_get_callback, this);
	evhttp_set_cb(server, "/relationUpdate", http_relation_update_callback, this);
	evhttp_set_cb(server, "/relationAdd", http_relation_add_callback, this);
	evhttp_set_cb(server, "/relationDelete", http_relation_delete_callback, this);
	evhttp_set_cb(server, "/relationMetaSet", http_relation_meta_set_callback, this);
	evhttp_set_cb(server, "/relationMetaUnset", http_relation_meta_unset_callback, this);
	evhttp_set_cb(server, "/dump", http_dump_callback, this);
	evhttp_set_cb(server, "/stats", http_stats_callback, this);

	event_base_dispatch(base);
}
