#include "stdafx.h"
#include "IGPython.h"
#include <string>

using namespace std;

extern "C"
{

BYTE execute_python_script(const char *pcFileName, const char *pcFuncName, int nNbArgs, const char **pcArgs)
{
	wstring wsPythonDllPath (L"");
	wsPythonDllPath += PYTHON3_DLL;
	HMODULE mod = ::LoadLibraryW (wsPythonDllPath.c_str());
	PPYUNICODE_FROMSTRING PyUnicode_FromString = (PPYUNICODE_FROMSTRING)::GetProcAddress (mod, "PyUnicode_FromString");
	PPYCALLABLE_CHECK PyCallable_Check = (PPYCALLABLE_CHECK)::GetProcAddress (mod, "PyCallable_Check");
	PPYTUPLE_NEW PyTuple_New = (PPYTUPLE_NEW)::GetProcAddress (mod, "PyTuple_New");
	PPYOBJECT_CALLOBJECT PyObject_CallObject = (PPYOBJECT_CALLOBJECT)::GetProcAddress (mod, "PyObject_CallObject");
	PPYTUPLE_SETITEM PyTuple_SetItem = (PPYTUPLE_SETITEM)::GetProcAddress (mod, "PyTuple_SetItem");
	PPY_INITIALIZE Py_Initialize = (PPY_INITIALIZE)::GetProcAddress (mod, "Py_Initialize");
	PPYIMPORT_IMPORT PyImport_Import = (PPYIMPORT_IMPORT)::GetProcAddress (mod, "PyImport_Import");
	PPYERR_OCCURRED PyErr_Occurred = (PPYERR_OCCURRED)::GetProcAddress (mod, "PyErr_Occurred");
	PPYLONG_ASLONG PyLong_AsLong = (PPYLONG_ASLONG)::GetProcAddress (mod, "PyLong_AsLong");
	PPYLONG_FROMLONG PyLong_FromLong = (PPYLONG_FROMLONG)::GetProcAddress (mod, "PyLong_FromLong");
	PPYERR_PRINT PyErr_Print = (PPYERR_PRINT)::GetProcAddress (mod, "PyErr_Print");
	PPY_FINALIZE Py_Finalize = (PPY_FINALIZE)::GetProcAddress (mod, "Py_Finalize");
	PPYOBJECT_GETATTRSTRING PyObject_GetAttrString = (PPYOBJECT_GETATTRSTRING)::GetProcAddress (mod, "PyObject_GetAttrString");

	PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    int i;

    Py_Initialize();
    pName = PyUnicode_FromString(pcFileName);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
	
	Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, pcFuncName);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(nNbArgs);
            for (i = 0; i < nNbArgs; ++i) {
                pValue = PyLong_FromLong(atoi(pcArgs[i + 3]));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", pcFuncName);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", pcFileName);
        return 1;
    }
    Py_Finalize();
    return 0;
}

}