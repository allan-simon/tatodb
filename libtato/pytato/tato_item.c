#include <Python.h>
#include "structmember.h"

#include "tato_item.h"


static void Item_dealloc(tatodb_ItemObject* self) {

	self->ob_type->tp_free((PyObject*)self);
};

static PyObject *Item_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    tatodb_ItemObject *self;

    self = (tatodb_ItemObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
		self->tatoItem = NULL;
        self->metas = NULL; 
    }
    return (PyObject *)self;
}

static int Item_init(tatodb_ItemObject *self, PyObject *args, PyObject *kwds)
{
    char *str=NULL;
	char *lang=NULL;

    static char *kwlist[] = {"string", "lang", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &str, &lang))
        return -1; 
	self->tatoItem = tato_item_new(
		0,
		tato_lang_new(lang),
		str,
		0
	);
    
    self->metas = PyObject_New(tatodb_DictObject, &tatodb_DictType);
    Py_XINCREF(self->metas);
    if (self->metas == NULL) {
        return -1;
    }
   
    return 0;
}


/**
 * Item.string get / set
 */
static PyObject *Item_getstring(tatodb_ItemObject *self, void *closure)
{
    printf("get String \n");
    return PyString_FromString(self->tatoItem->str);
}

static int Item_setstring(tatodb_ItemObject *self, PyObject *value, void *closure)
{
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the first attribute");
    return -1;
  }
  
  if (! PyString_Check(value)) {
    PyErr_SetString(PyExc_TypeError, 
                    "The first attribute value must be a string");
    return -1;
  }
      
  self->tatoItem->str = strdup(PyString_AsString(value));    

  return 0;
}


/**
 * Item.id get / set
 */
static PyObject *Item_getid(tatodb_ItemObject *self, void *closure)
{
    return PyInt_FromLong((long)self->tatoItem->id);
}

static int Item_setid(tatodb_ItemObject *self, PyObject *value, void *closure)
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
      
  self->tatoItem->id = (TatoItemId) PyInt_AsLong(value);    

  return 0;
}


/**
 * Item.metas get / set
 */
static tatodb_DictObject *Item_getmetas(tatodb_ItemObject *self, void *closure)
{
    printf("get\n");
    //self->tatoItem->metas = *(self->metas->tatoDict);
	return self->metas;
}

static int Item_setmetas(tatodb_ItemObject *self, PyObject *value, void *closure)
{
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the first attribute");
    return -1;
  }

  return 0;
}




static PyMemberDef Item_members[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef Item_methods[] = {
    {NULL}  /* Sentinel */
};

static PyGetSetDef Item_getseters[] = {
    {"string", 
     (getter)Item_getstring, (setter)Item_setstring,
     "Item string",
     NULL},
    {"id", 
     (getter)Item_getid, (setter)Item_setid,
     "Item id",
     NULL},
    {"metas", 
     (getter)Item_getmetas, (setter)Item_setmetas,
     "Item metas",
     NULL},

	/*
    {"lang", 
     (getter)Item_getlang, (setter)Item_setlang,
     "Item language",
     NULL},
	*/
    {NULL}  /* Sentinel */
};

PyTypeObject tatodb_ItemType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "tatodb.item",             /*tp_name*/
    sizeof(tatodb_ItemObject),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Item_dealloc, /*tp_dealloc*/
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
    "Item objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Item_methods,             /* tp_methods */
    Item_members,             /* tp_members */
    Item_getseters,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Item_init,      /* tp_init */
    0,                         /* tp_alloc */
    Item_new,                 /* tp_new */
};

