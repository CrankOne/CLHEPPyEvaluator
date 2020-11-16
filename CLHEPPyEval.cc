#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <CLHEP/Evaluator/Evaluator.h>

// TODO-1-1
//  These definitions should be the named class constants,
//  but I don't know the similar mechanism in Python C API. So far, added them
//  as module's int contants.
// TODO-1-2
//  Imposing Python functions at a runtime should be feasible -- using
//  closures and JIT, but the particular use case scenario is vague. At least,
//  more useful and simple way would be to extend the Evaluator code with
//  stateful variables.

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

// Wraps
//      void print_error() const;
static PyObject *
CLHEPEvaluator_print_error( CLHEPEvaluator_t * self
                          , PyObject *Py_UNUSED(ignored) ) {
    self->evaluatorPtr->print_error();
    Py_RETURN_NONE;
}

// Wraps
//      std::string error_name() const;
static PyObject *
CLHEPEvaluator_error_name( CLHEPEvaluator_t * self
                         , PyObject *Py_UNUSED(ignored) ) {
    const std::string s = self->evaluatorPtr->error_name();
    return PyUnicode_FromStringAndSize( s.data(), s.size() );
}

// Wraps
//      void setVariable(const char * name, double value);
//      void setVariable(const char * name, const char * expression);
static PyObject *
CLHEPEvaluator_set_variable( CLHEPEvaluator_t * self
                           , PyObject * const * args
                           , Py_ssize_t nargs ) {
    const char * name, * expression;
    double value;
    if( 2 != nargs ) {
        PyErr_SetString( PyExc_TypeError
                       , "Wrong number of arguments (2 expected)." );
        return NULL;
    }
    if( !PyUnicode_Check(args[0]) ) {
        PyErr_SetString( PyExc_TypeError
                       , "First argument is not a string." );
        return NULL;
    }
    name = PyUnicode_AsUTF8( args[0] );
    if( PyUnicode_Check(args[1]) ) {
        expression = PyUnicode_AsUTF8(args[1]);
        self->evaluatorPtr->setVariable(name, expression);
    } else {
        value = PyFloat_AsDouble(args[1]);
        if( -1 == value && PyErr_Occurred() ) {
            PyErr_SetString( PyExc_TypeError
                           , "Unable to set variable: second argument is not"
                             " a number, nor a string." );
            return NULL;
        }
        self->evaluatorPtr->setVariable(name, value);
    }
    Py_RETURN_NONE;
}

// Wraps
//      bool findVariable(const char * name) const;
static PyObject *
CLHEPEvaluator_find_variable( CLHEPEvaluator_t * self
                            , PyObject * varName_ ) {
    const char * varName;
    if( !PyUnicode_Check(varName_) ) {
        PyErr_SetString(PyExc_TypeError, "String identifier expected.");
        return NULL;
    }
    varName = PyUnicode_AsUTF8(varName_);
    if( self->evaluatorPtr->findVariable(varName) ) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Wraps
//      void removeVariable(const char * name);
static PyObject *
CLHEPEvaluator_remove_variable( CLHEPEvaluator_t * self
                              , PyObject * varName_ ) {
    const char * varName;
    if( !PyUnicode_Check(varName_) ) {
        PyErr_SetString(PyExc_TypeError, "String identifier expected.");
        return NULL;
    }
    varName = PyUnicode_AsUTF8(varName_);
    self->evaluatorPtr->removeVariable(varName);
    Py_RETURN_NONE;
}

// Wraps
//      void clear();
static PyObject *
CLHEPEvaluator_clear( CLHEPEvaluator_t * self
                    , PyObject *Py_UNUSED(ignored) ) {
    self->evaluatorPtr->clear();
    Py_RETURN_NONE;
}

// Wraps
//      void setStdMath();
static PyObject *
CLHEPEvaluator_set_std_math( CLHEPEvaluator_t * self
                           , PyObject *Py_UNUSED(ignored) ) {
    self->evaluatorPtr->setStdMath();
    Py_RETURN_NONE;
}

// Wraps
//      void setSystemOfUnits(double meter, double kilogram, double second, double ampere, double kelvin, double mole, double);
static PyObject *
CLHEPEvaluator_set_system_of_units( CLHEPEvaluator_t * self
                                  , PyObject * args
                                  , PyObject * kwargs) {
    int rc;
    static const char * keywords[] = {
        "meter", "kilogram", "second", "ampere",
        "kelvin", "mole", "candela", NULL
        };
    double meter = 1
         , kilogram = 1
         , second = 1
         , ampere = 1
         , kelvin = 1
         , mole = 1
         , candela = 1
         ;
    rc = PyArg_ParseTupleAndKeywords( args, kwargs, "|ddddddd", const_cast<char**>(keywords)
            , &meter, &kilogram, &second, &ampere, &kelvin, &mole, &candela );
    if(!rc) return NULL;
    self->evaluatorPtr->setSystemOfUnits( meter, kilogram, second
            , ampere, kelvin, mole, candela);
    Py_RETURN_NONE;
}

// Wraps
//      bool findFunction(const char * name, int npar) const;
static PyObject *
CLHEPEvaluator_find_function( CLHEPEvaluator_t * self
                            , PyObject * const * args
                            , Py_ssize_t nargs ) {
    const char * funcName;
    int nArgs;

    if( 2 != nargs ) {
        PyErr_SetString( PyExc_TypeError
                       , "Wrong number of arguments (2 expected)." );
        return NULL;
    }
    if( !PyUnicode_Check(args[0]) ) {
        PyErr_SetString( PyExc_TypeError
                       , "First argument is not a string." );
        return NULL;
    }
    funcName = PyUnicode_AsUTF8( args[0] );
    if( PyLong_Check(args[1]) ) {
        PyErr_SetString( PyExc_TypeError
                       , "Second argument is not an integer." );
        return NULL;
    }
    nargs = (int) PyLong_AsLong(args[1]);
    if( self->evaluatorPtr->findFunction(funcName, nargs) ) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Wraps
//      void removeFunction(const char * name, int npar);
static PyObject *
CLHEPEvaluator_remove_function( CLHEPEvaluator_t * self
                            , PyObject * const * args
                            , Py_ssize_t nargs ) {
    const char * funcName;
    int nArgs;

    if( 2 != nargs ) {
        PyErr_SetString( PyExc_TypeError
                       , "Wrong number of arguments (2 expected)." );
        return NULL;
    }
    if( !PyUnicode_Check(args[0]) ) {
        PyErr_SetString( PyExc_TypeError
                       , "First argument is not a string." );
        return NULL;
    }
    funcName = PyUnicode_AsUTF8( args[0] );
    if( PyLong_Check(args[1]) ) {
        PyErr_SetString( PyExc_TypeError
                       , "Second argument is not an integer." );
        return NULL;
    }
    nargs = (int) PyLong_AsLong(args[1]);
    self->evaluatorPtr->removeFunction(funcName, nargs);
    Py_RETURN_NONE;
}

// TODO-1-2:
// void setFunction(const char * name, double (*fun)());
// void setFunction(const char * name, double (*fun)(double));
// void setFunction(const char * name, double (*fun)(double,double));
// void setFunction(const char * name, double (*fun)(double,double,double));
// void setFunction(const char * name, double (*fun)(double,double,double,double));
// void setFunction(const char * name, double (*fun)(double,double,double,double,double));

/*
 * Python C interfacing
 */

static PyMethodDef CLHEPEvaluator_methods[] = {
    {"evaluate"
        , (PyCFunction) CLHEPEvaluator_evaluate
        , METH_O
        , "Evaluates the arithmetic expression given as character string. "
          "The expression may consist of numbers, variables and functions "
          "separated by arithmetic (+, - , /, *, ^, **) and logical "
          " operators (==, !=, >, >=, <, <=, &&, ||)."
    },
    {"status"
        , (PyCFunction) CLHEPEvaluator_status
        , METH_NOARGS
        , "Returns status of the last operation with the evaluator."
    },
    {"error_position"
        , (PyCFunction) CLHEPEvaluator_error_position
        , METH_NOARGS
        , "Returns position in the input string where the problem occured."
    },
    {"print_error"
        , (PyCFunction) CLHEPEvaluator_print_error
        , METH_NOARGS
        , "Prints error message if status() is an ERROR."
    },
    {"error_name"
        , (PyCFunction) CLHEPEvaluator_error_name
        , METH_NOARGS
        , "Get a string defining the error name."
    },
    {"set_variable"
        , (PyCFunction) CLHEPEvaluator_set_variable
        , METH_FASTCALL
        , "Adds to the dictionary a variable with an arithmetic expression"
          " assigned to it."
          " If a variable with such a name already exist in the dictionary,"
          " then status will be set to WARNING_EXISTING_VARIABLE."
    },
    {"find_variable"
        , (PyCFunction) CLHEPEvaluator_find_variable
        , METH_O
        , "Finds the variable in the dictionary."
    },
    {"remove_variable"
        , (PyCFunction) CLHEPEvaluator_remove_variable
        , METH_O
        , " Removes the variable from the dictionary."
    },
    {"clear"
        , (PyCFunction) CLHEPEvaluator_clear
        , METH_NOARGS
        , "Clear all settings."
    },
    {"set_std_math"
        , (PyCFunction) CLHEPEvaluator_set_std_math
        , METH_NOARGS
        , "Sets standard mathematical functions and constants."
    },
    {"set_system_of_units"
        , (PyCFunction) CLHEPEvaluator_set_system_of_units
        , METH_VARARGS | METH_KEYWORDS
        , "Sets system of units. Default is the SI system of units "
          "(meter, kilogram, second, ampere, kelvin, mole, candela)."
          " To set the CGS (Centimeter-Gram-Second) system of units"
          " one should call:\n"
          "    setSystemOfUnits(100., 1000., 1.0, 1.0, 1.0, 1.0, 1.0);\n"
          " To set system of units accepted in the GEANT4 simulation toolkit"
          " one should call:\n"
          "    setSystemOfUnits(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10, 1.0, 1.0, 1.0);"
    },
    // functions
    {"find_function"
        , (PyCFunction) CLHEPEvaluator_find_function
        , METH_FASTCALL
        , "Finds the function in the dictionary."
    },
    {"remove_function"
        , (PyCFunction) CLHEPEvaluator_remove_function
        , METH_FASTCALL
        , "Removes the function from the dictionary."
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





