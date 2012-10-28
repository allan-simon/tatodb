#include <tato/hyperitem.h>
#include <tato/hyperrelation.h>

int main(int argc, char *argv[]) {


	TatoHyperItemLang *lang = tato_hyper_item_lang_new("fra");

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

	// we link 1 to itself 
	//
	//  ø=========ø
	//  |		 |
	//  v		 |
	//  1 ========ø==>2
	tato_hyper_relation_link_add(relation, item);
	tato_hyper_relation_flags_set(relation, 4);

	// we merge 1 into 2
	//
	//
	//  ø=========ø
	//  |		 |
	//  v		 |
	//  2 ========ø
	tato_hyper_item_merge_into(item, item2);
	tato_hyper_item_dump(item2, stdout);
	tato_hyper_relation_dump(relation, stdout);
	tato_hyper_item_delete(item2);
	tato_hyper_item_lang_free(lang);
	return 0;
}
