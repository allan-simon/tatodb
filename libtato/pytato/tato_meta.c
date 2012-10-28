#include <Python.h>

#include "tato_meta.h"


static void Dict_dealloc(tatodb_DictObject* self) {
    
    tato_kvlist_clear(&self->tatoDict);
	self->ob_type->tp_free((PyObject*)self);
};

static PyObject *Dict_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    tatodb_DictObject *self;

    self = (tatodb_DictObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
		self->tatoDict = NULL;
    }
    return (PyObject *)self;
}

static int Dict_init(tatodb_DictObject *self, PyObject *args, PyObject *kwds)
{
	self->tatoDict = NULL;
    return 0;
}
/**
 * Get
 */
static PyObject * Dict_subscript(tatodb_DictObject *self, register PyObject *pykey)
{
    char* key = NULL;
    const char* value = NULL;
    if (! PyString_Check(pykey)) {
        PyErr_SetString(
            PyExc_TypeError, 
            "The first attribute value must be a string"
        );
        return NULL;
    }

    printf("I'm going to die\n");

    key= strdup(PyString_AsString(pykey));
    value = tato_kvlist_get(self->tatoDict, key);
    if (value == NULL) {
        return NULL;
    }
    value = strdup(value);
    return PyString_FromString(value); 
}
/**
 * set
 */
static int Dict_ass_sub(tatodb_DictObject *self, PyObject *pykey, PyObject *pyvalue)
{
    char* key = NULL;
    char* value = NULL;

    if (pyvalue == NULL) {
        return tato_kvlist_delete(
            &self->tatoDict,
            strdup(PyString_AsString(pykey))
        );
    } else {
        if (! PyString_Check(pyvalue)) {
            PyErr_SetString(
                PyExc_TypeError, 
                "The first attribute value must be a string"
            );
            return -1;
        }
        key = strdup(PyString_AsString(pykey));
        value = strdup(PyString_AsString(pyvalue));
        if(!tato_kvlist_add(&self->tatoDict, key, value)) {
            return -1;
        } 
    }
    return 0;
}

static PyMappingMethods Dict_as_mapping = {
    NULL, /*mp_length*/
    (binaryfunc) Dict_subscript, /*mp_subscript*/
    (objobjargproc)Dict_ass_sub, /*mp_ass_subscript*/
};


PyTypeObject tatodb_DictType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "tatodb.dict",             /*tp_name*/
    sizeof(tatodb_DictObject),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Dict_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &Dict_as_mapping,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Dict objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Dict_init,      /* tp_init */
    0,                         /* tp_alloc */
    Dict_new,                 /* tp_new */
};

