#include <tato/hyperitem.h>
#include <tato/hyperrelation.h>
#include <tato/hyperdb.h>

int main(int argc, char *argv[]) {
    TatoHyperDb* tatoHyperDb = tato_hyper_db_new();

	TatoHyperItemLang *lang = tato_hyper_item_lang_new("fra");


	TatoHyperItem *item = tato_hyper_db_item_add(
        tatoHyperDb,
        "fra",
		"un",
        0
    );


	TatoHyperItem *item2 = tato_hyper_db_item_add(
        tatoHyperDb,
        "fra",
		"deux",
        0
    );


    tato_hyper_db_relation_add(
        tatoHyperDb,
        1,//id of item
        2,//id of item2
        1, //SHDICT_TRANSLATION_REL_FLAG
        0
    );

    tato_hyper_db_relation_add(
        tatoHyperDb,
        2,//id of item2
        1,//id of item
        1, //SHDICT_TRANSLATION_REL_FLAG
        0
    );

    tato_hyper_db_item_delete(
        tatoHyperDb,
        1
    );

    tato_hyper_db_item_delete(
        tatoHyperDb,
        2
    );

    tato_hyper_db_free(
        tatoHyperDb
    );

    /*
	TatoHyperItem *item = tato_hyper_item_new(
		1,
		lang,
		"bonjour je suis nono le petit robot",
		0
	);


	TatoHyperItem *item2 = tato_hyper_item_new(
		2,
		lang,
		"bonjour je toto suis nono le petit robot",
		0
	);

	// we link  1 ====> 2 
	TatoHyperRelation *relation = tato_hyper_relation_new(
		42,
		item,
		item2,
		0,
		0
	);

	// we link  2 ====> 1 
	TatoHyperRelation *relation2 = tato_hyper_relation_new(
		43,
		item2,
		item,
		0,
		0
	);


	tato_hyper_item_delete(item);
	tato_hyper_item_delete(item2);
    */
	tato_hyper_item_lang_free(lang);

    return 0;
}

