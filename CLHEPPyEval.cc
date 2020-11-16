#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <CLHEP/Evaluator/Evaluator.h>

// TODO-1-1
//  these should be the named class constants (static class consts)
//  but I don't know the similar mechanism in Python C API, so added them
//  as module's int contants here

// Pythonic object referencing C++ instance of HepTool::Evaluator
typedef struct {
    PyObject_HEAD;
    HepTool::Evaluator * evaluatorPtr;  // Pointer to native HepTool::Evaluator instance
} CLHEPEvaluator_t;

// Constructor
static PyObject *
CLHEPEvaluator_new( PyTypeObject * type
                  , PyObject * args
                  , PyObject * kwds ) {
    CLHEPEvaluator_t * self = (CLHEPEvaluator_t *) type->tp_alloc(type, 0);
    if( self != NULL ) {
        self->evaluatorPtr = NULL;
    }
    self->evaluatorPtr = new HepTool::Evaluator();
    return (PyObject *) self;
}

// Destructor
static void
CLHEPEvaluator_dealloc(CLHEPEvaluator_t * self) {
    if( self->evaluatorPtr ) {
        delete self->evaluatorPtr;
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
}

#if 0  // XXX does not have any parameterisation
static int
CLHEPEvaluator_init( CLHEPEvaluator_t * self
               , PyObject * args
               , PyObject * kwds ) {
    // ?..
    return 0;
}
#endif

// Wraps
//      double evaluate(const char * expression);
static PyObject *
CLHEPEvaluator_evaluate( CLHEPEvaluator_t * self
                       , PyObject * expression_ ) {
    int rc;
    const char * expression;
    double result;
    // assure type
    if( !PyUnicode_Check(expression_) ) {
        PyErr_SetString( PyExc_TypeError
                       , "A string argument expected." );
        return NULL;
    }
    // retrieve C string
    expression = PyUnicode_AsUTF8( expression_ );
    // eval (TODO: check?)
    result = self->evaluatorPtr->evaluate(expression);
    // return result
    return Py_BuildValue("d", result);
}

// Wraps
//      int status() const;
static PyObject *
CLHEPEvaluator_status( CLHEPEvaluator_t * self
                     , PyObject *Py_UNUSED(ignored) ) {
    return PyLong_FromLong( self->evaluatorPtr->status() );
}

// Wraps
//      int error_position() const;
static PyObject *
CLHEPEvaluator_error_position( CLHEPEvaluator_t * self
                             , PyObject *Py_UNUSED(ignored) ) {
    return PyLong_FromLong( self->evaluatorPtr->error_position() );
}

// void print_error() const;
// std::string error_name() const;
// void setVariable(const char * name, double value);
// void setVariable(const char * name, const char * expression);
// void setFunction(const char * name, double (*fun)());
// void setFunction(const char * name, double (*fun)(double));
// void setFunction(const char * name, double (*fun)(double,double));
// void setFunction(const char * name, double (*fun)(double,double,double));
// void setFunction(const char * name, double (*fun)(double,double,double,double));
// void setFunction(const char * name, double (*fun)(double,double,double,double,double));
// bool findVariable(const char * name) const;
// bool findFunction(const char * name, int npar) const;
// void removeVariable(const char * name);
// void removeFunction(const char * name, int npar);
// void clear();
// void setStdMath();
// void setSystemOfUnits(double meter, double kilogram, double second, double ampere, double kelvin, double mole, double);

/*
 * Python C interfacing
 */

static PyMethodDef CLHEPEvaluator_methods[] = {
    {"evaluate", (PyCFunction) CLHEPEvaluator_evaluate, METH_O,
        "Evaluates the arithmetic expression given as character string. "
        "The expression may consist of numbers, variables and functions "
        "separated by arithmetic (+, - , /, *, ^, **) and logical "
        " operators (==, !=, >, >=, <, <=, &&, ||)."
    },
    {"status", (PyCFunction) CLHEPEvaluator_status, METH_NOARGS,
        "Returns status of the last operation with the evaluator."
    },
    {"error_position", (PyCFunction) CLHEPEvaluator_error_position, METH_NOARGS,
        "Returns position in the input string where the problem occured."
    },
    // ...
    {NULL}  /* Sentinel */
};


static PyTypeObject CLHEPEvaluator = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CLHEPEvaluator.Evaluator",
    .tp_basicsize = sizeof(CLHEPEvaluator_t),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor) CLHEPEvaluator_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "A Python interface to native CLHEP evaulator class.",
    .tp_methods = CLHEPEvaluator_methods,
    //.tp_as_sequence = &TracksData_sequence_interface,
    //.tp_init = (initproc) CLHEPEvaluator_init,
    .tp_new = CLHEPEvaluator_new,
    //.tp_members = TracksData_members,
    //.tp_getset = TracksData_getsetters
};

static PyModuleDef CLHEPEvaluatorModule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "CLHEPEvaluator",
    .m_doc = "Python wrapper for native CLHEP Evaluator object.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_CLHEPEvaluator(void) {
    PyObject *m;
    if( PyType_Ready(&CLHEPEvaluator) < 0 )
        return NULL;
    m = PyModule_Create(&CLHEPEvaluatorModule);
    if (m == NULL)
        return NULL;

    // Add constants (see TODO-1-1)
    {
        int rc;
        #define ADD_NUMERIC_CONST(name)                                     \
        rc = PyModule_AddIntConstant( m, #name, HepTool::Evaluator:: name );  \
        if(0 != rc) return NULL;
        ADD_NUMERIC_CONST( OK );
        ADD_NUMERIC_CONST( WARNING_EXISTING_VARIABLE );
        ADD_NUMERIC_CONST( WARNING_EXISTING_FUNCTION );
        ADD_NUMERIC_CONST( WARNING_BLANK_STRING );
        ADD_NUMERIC_CONST( ERROR_NOT_A_NAME );
        ADD_NUMERIC_CONST( ERROR_SYNTAX_ERROR );
        ADD_NUMERIC_CONST( ERROR_UNPAIRED_PARENTHESIS );
        ADD_NUMERIC_CONST( ERROR_UNEXPECTED_SYMBOL );
        ADD_NUMERIC_CONST( ERROR_UNKNOWN_VARIABLE );
        ADD_NUMERIC_CONST( ERROR_UNKNOWN_FUNCTION );
        ADD_NUMERIC_CONST( ERROR_EMPTY_PARAMETER );
        ADD_NUMERIC_CONST( ERROR_CALCULATION_ERROR );
        #undef ADD_NUMERIC_CONST
    }

    // Add Evaluator type
    Py_INCREF(&CLHEPEvaluator);
    PyModule_AddObject(m, "Evaluator", (PyObject *) &CLHEPEvaluator);

    return m;
}





