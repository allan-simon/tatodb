#ifndef _PY_TATO_RELATION_H
#define _PY_TATO_RELATION_H

#include <tato/relation.h>
#include <tato/lang.h>


#include "tato_item.h"
#include "tato_module.h"

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	TatoRelation *tatoRelation;
} tatodb_RelationObject;


#endif
