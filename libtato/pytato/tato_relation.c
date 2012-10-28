#include <Python.h>
#include "structmember.h"

#include "tato_relation.h"

static void Relation_dealloc(tatodb_RelationObject* self) {
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *Relation_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    tatodb_RelationObject *self;

    self = (tatodb_RelationObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
		self->tatoRelation = NULL;
    }
    return (PyObject *)self;
}


static int Relation_init(tatodb_RelationObject *self, PyObject *args, PyObject *kwds)
{
    return 0;
}


/**
 * Relation.id get / set
 */
static PyObject *Relation_getid(tatodb_RelationObject *self, void *closure)
{
    return PyInt_FromLong((long)self->tatoRelation->id);
}

static int Relation_setid(tatodb_RelationObject *self, PyObject *value, void *closure)
{
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the first attribute");
    return -1;
  }
  
  if (! PyInt_Check(value)) {
    PyErr_SetString(PyExc_TypeError, 
                    "The first attribute value must be a id");
    return -1;
  }
      
  self->tatoRelation->id = (TatoRelationId) PyInt_AsLong(value);    

  return 0;
}


/**
 * Relation.start get / set
 */
static tatodb_ItemObject *Relation_getstart(tatodb_RelationObject *self, void *closure)
{

	tatodb_ItemObject* newItemObject = NULL;
	newItemObject = PyObject_New(tatodb_ItemObject, &tatodb_ItemType);
	if (newItemObject == NULL) {
		return NULL;
	}
	newItemObject->tatoItem = self->tatoRelation->x;

	return newItemObject;
}

static int Relation_setstart(tatodb_RelationObject *self, PyObject *value, void *closure)
{
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the first attribute");
    return -1;
  }

  return 0;
}




static PyMemberDef Relation_members[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef Relation_methods[] = {
    {NULL}  /* Sentinel */
};

static PyGetSetDef Relation_getseters[] = {
	{"start", 
     (getter)Relation_getstart, (setter)Relation_setstart,
     "Relation start Item",
     NULL},
	 /*
    {"end", 
     (getter)Relation_getend, (setter)Relation_setend,
     "Relation end Item",
     NULL},
	 */
    {"id", 
     (getter)Relation_getid, (setter)Relation_setid,
     "Relation id",
     NULL},
    {NULL}  /* Sentinel */
};

PyTypeObject tatodb_RelationType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "tatodb.relation",             /*tp_name*/
    sizeof(tatodb_RelationObject),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Relation_dealloc, /*tp_dealloc*/
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
    "Relation objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Relation_methods,             /* tp_methods */
    Relation_members,             /* tp_members */
    Relation_getseters,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Relation_init,      /* tp_init */
    0,                         /* tp_alloc */
    Relation_new,                 /* tp_new */
};


