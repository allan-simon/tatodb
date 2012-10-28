#ifndef _PY_TATO_ITEM_H
#define _PY_TATO_ITEM_H

#include <tato/item.h>
#include <tato/lang.h>

#include "tato_module.h"
#include "tato_meta.h"

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	TatoItem *tatoItem;
    tatodb_DictObject *metas; 
} tatodb_ItemObject;


#endif
