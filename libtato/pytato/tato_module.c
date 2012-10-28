#include <Python.h>

#include "tato_module.h"
/**
 * MODULE PART
 *
 */

static PyMethodDef tatodb_methods[] = {
	 {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
inittatodb(void) 
{
	PyObject* m;

	if (PyType_Ready(&tatodb_ItemType) < 0)
		return;

	if (PyType_Ready(&tatodb_DictType) < 0)
		return;

	if (PyType_Ready(&tatodb_DbType) < 0)
		return;

	if (PyType_Ready(&tatodb_RelationType) < 0)
		return;

	m = Py_InitModule4(
		"tatodb",
		tatodb_methods,
		"Example module that creates an extension type.",
        (PyObject*) NULL,
        PYTHON_API_VERSION
	);

	Py_INCREF(&tatodb_ItemType);
	PyModule_AddObject(m, "Item", (PyObject *)&tatodb_ItemType);

	Py_INCREF(&tatodb_RelationType);
	PyModule_AddObject(m, "Relation", (PyObject *)&tatodb_RelationType);

	Py_INCREF(&tatodb_DictType);
	PyModule_AddObject(m, "Dict", (PyObject *)&tatodb_DictType);

	Py_INCREF(&tatodb_DbType);
	PyModule_AddObject(m, "Db", (PyObject *)&tatodb_DbType);
}
