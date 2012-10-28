#include <Python.h>
#include "structmember.h"

#include "tato_db.h"

/**
 * Routine Part 
 *
 */
static void Db_dealloc(tatodb_DbObject* self) {
	if (self->tatoDb) {
		tato_db_free(self->tatoDb);
	}
	self->ob_type->tp_free((PyObject*)self);
};

static PyObject *Db_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    tatodb_DbObject *self;

    self = (tatodb_DbObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
		self->tatoDb = NULL;
    }
    return (PyObject *)self;
}

static int Db_init(tatodb_DbObject *self, PyObject *args, PyObject *kwds)
{

    self->tatoDb = tato_db_new();
    return 0;
}

/**
 * Internal Structure Part
 *
 */

/**
 * Db.add_item(lang, string) 
 */
static tatodb_ItemObject *Db_add_item(tatodb_DbObject *self, PyObject *args, PyObject *kwds)
{
	TatoItem* newItem = NULL; 
	tatodb_ItemObject* newItemObject = NULL;
    char *str=NULL;
	char *lang=NULL;

    static char *kwlist[] = {"lang", "string", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &lang, &str)) {
        return NULL;
	}

	newItem = tato_db_item_add(self->tatoDb, lang, str, 0);
	if (newItem == NULL) {
		return NULL;
	}

	newItemObject = PyObject_New(tatodb_ItemObject, &tatodb_ItemType);
	if (newItemObject == NULL) {
		return NULL;
	}
	newItemObject->tatoItem = newItem;

	return newItemObject;
}

/**
 * Db.get_item(id) 
 */
static tatodb_ItemObject *Db_get_item(tatodb_DbObject *self, PyObject *args)
{
	TatoItemId itemId = 0;
	TatoItem* newItem = NULL; 
	tatodb_ItemObject* newItemObject = NULL;


    if (! PyArg_ParseTuple(args, "I", &itemId)) {
        return NULL;
	}

	newItem = tato_db_item_find(self->tatoDb, itemId);
	if (newItem == NULL) {
		return NULL;
	}

	newItemObject = PyObject_New(tatodb_ItemObject, &tatodb_ItemType);
	if (newItemObject == NULL) {
		return NULL;
	}
	newItemObject->tatoItem = newItem;

	return newItemObject;
}

/**
 * Db.add_relation(startId, endId) 
 */
static tatodb_RelationObject *Db_add_relation(tatodb_DbObject *self, PyObject *args, PyObject *kwds)
{
	TatoRelation* newRelation = NULL; 
	tatodb_RelationObject* newRelationObject = NULL;
    TatoItemId start = 0;
	TatoItemId end = 0;

    static char *kwlist[] = {"start", "end", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "II", kwlist, &start, &end)) {
        return NULL;
	}

	newRelation = tato_db_relation_add(self->tatoDb, start, end, 0, 0);
	if (newRelation == NULL) {
		return NULL;
	}

	newRelationObject = PyObject_New(tatodb_RelationObject, &tatodb_RelationType);
	if (newRelationObject == NULL) {
		return NULL;
	}
	newRelationObject->tatoRelation = newRelation;

	return newRelationObject;
}

/**
 * Db.get_relation(id) 
 */
static tatodb_RelationObject *Db_get_relation(tatodb_DbObject *self, PyObject *args)
{
	TatoRelationId relationId = 0;
	TatoRelation* newRelation = NULL; 
	tatodb_RelationObject* newRelationObject = NULL;


    if (! PyArg_ParseTuple(args, "I", &relationId)) {
        return NULL;
	}

	newRelation = tato_db_relation_find(self->tatoDb, relationId);
	if (newRelation == NULL) {
		return NULL;
	}

	newRelationObject = PyObject_New(tatodb_RelationObject, &tatodb_RelationType);
	if (newRelationObject == NULL) {
		return NULL;
	}
	newRelationObject->tatoRelation = newRelation;

	return newRelationObject;
}



static PyMemberDef Db_members[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef Db_methods[] = {
    {"add_item", (PyCFunction)Db_add_item, METH_KEYWORDS, "Add a new item in the DB"},
    {"get_item", (PyCFunction)Db_get_item, METH_VARARGS, "Add a new item in the DB"},
    {"add_relation", (PyCFunction)Db_add_relation, METH_KEYWORDS, "Add a new relation in the DB"},
    {"get_relation", (PyCFunction)Db_get_relation, METH_VARARGS, "Add a new relation in the DB"},
    {NULL}  /* Sentinel */
};

static PyGetSetDef Db_getseters[] = {
    {NULL}  /* Sentinel */
};



PyTypeObject tatodb_DbType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "tatodb.item",             /*tp_name*/
    sizeof(tatodb_DbObject),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Db_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Db objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Db_methods,             /* tp_methods */
    Db_members,             /* tp_members */
    Db_getseters,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Db_init,      /* tp_init */
    0,                         /* tp_alloc */
    Db_new,                 /* tp_new */
};

