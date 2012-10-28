
#ifndef _PY_TATO_DB_H
#define _PY_TATO_DB_H

#include <tato/db.h>
#include <tato/lang.h>

#include "tato_item.h"
#include "tato_relation.h"
#include "tato_module.h"

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	TatoDb *tatoDb;
} tatodb_DbObject;


#endif
