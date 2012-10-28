#ifndef _PY_TATO_DICT_H
#define _PY_TATO_DICT_H

#include <tato/kvlist.h>

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	TatoKvList tatoDict;
} tatodb_DictObject;


#endif
