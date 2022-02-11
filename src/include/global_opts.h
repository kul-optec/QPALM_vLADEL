/**
 * @file global_opts.h
 * @author Ben Hermans
 * @brief Custom memory allocation, print and utility functions, and data types for floats and ints.
 * @details Memory allocation and print functions depend on whether the code is compiled as a standalone
 * library or with matlab or python. The data types used for floating point numbers and integer numbers
 * can be changed here as well. Finally, some customized operations (max, min, mod and abs) are included
 * as well.
 */

#ifndef GLOBAL_OPTS_H
# define GLOBAL_OPTS_H

# ifdef __cplusplus
extern "C" {
# endif 

#include "ladel.h"
typedef ladel_double  c_float; /**< type for floating point numbers */
typedef ladel_int     c_int; /**< type for integer numbers */


/**
 * @}
 */

/** 
 * @name Data customizations
 * @{
 */
#  include <stdlib.h>

/**
 * @}
 */

/**
 * @name Custom memory allocation (e.g. matlab/python)
 * @{
 */
#  ifdef MATLAB
    #   include "mex.h"
static void* c_calloc(size_t num, size_t size) {
  void *m = mxCalloc(num, size);
  mexMakeMemoryPersistent(m);
  return m;
}

static void* c_malloc(size_t size) {
  void *m = mxMalloc(size);

  mexMakeMemoryPersistent(m);
  return m;
}

static void* c_realloc(void *ptr, size_t size) {
  void *m = mxRealloc(ptr, size);

  mexMakeMemoryPersistent(m);
  return m;
}

    #   define c_free mxFree
#  elif defined PYTHON

// Define memory allocation for python. Note that in Python 2 memory manager
// Calloc is not implemented
    #   include <Python.h>
    #   define c_malloc PyMem_Malloc
    #   if PY_MAJOR_VERSION >= 3
    #    define c_calloc PyMem_Calloc
    #   else  /* if PY_MAJOR_VERSION >= 3 */
static void* c_calloc(size_t num, size_t size) {
  void *m = PyMem_Malloc(num * size);

  memset(m, 0, num * size);
  return m;
}

    #   endif /* if PY_MAJOR_VERSION >= 3 */

    #   define c_free PyMem_Free
    #   define c_realloc PyMem_Realloc
#  else  /* if not MATLAB of Python */
    #   define c_malloc malloc    /**< custom malloc */
    #   define c_calloc calloc    /**< custom calloc */
    #   define c_free free        /**< custom free */
    #   define c_realloc realloc  /**< custom realloc */

#  endif /* ifdef MATLAB */

/**
 * @}
 */


/* PRINTING */
# ifdef PRINTING
#  include <stdio.h>
#  include <string.h>

#if defined(_WIN32) && defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

#  ifdef MATLAB
#   define c_print mexPrintf

#  elif defined PYTHON
#   include <Python.h>
#   define c_print(...) {PyGILState_STATE gstate; int py_check = PyGILState_Check(); if (!py_check) {gstate = PyGILState_Ensure(); PySys_WriteStdout(__VA_ARGS__); PyGILState_Release(gstate);}}
#  elif defined R_LANG
#   include <R_ext/Print.h>
#   define c_print Rprintf
#  else  /* ifdef MATLAB */
#   define c_print printf
#  endif /* ifdef MATLAB */

// Print error macro
#  define c_eprint(...) c_print("ERROR in %s: ", __FUNCTION__); c_print( \
    __VA_ARGS__); c_print("\n");

# endif /* ifdef PRINTING */


/**
 * @name Custom operations
 * @{
 */
# ifndef c_absval
#  define c_absval(x) (((x) < 0) ? -(x) : (x)) /**< absolute value */
# endif /* ifndef c_absval */

# ifndef c_max
#  define c_max(a, b) (((a) > (b)) ? (a) : (b)) /**< maximum of two values */
# endif /* ifndef c_max */

# ifndef c_min
#  define c_min(a, b) (((a) < (b)) ? (a) : (b)) /**< minimum of two values */
# endif /* ifndef c_min */

# ifndef mod
#  define mod(a,b) ((((a)%(b))+(b))%(b)) /**< modulo operation (positive result for all values) */
#endif

#include <math.h>
#ifdef DFLOAT
#define c_sqrt sqrtf /**< square root */
#define c_acos acosf /**< arc cosine */
#define c_cos  cosf  /**< cosine */
#else
#define c_sqrt sqrt /**< square root */
#define c_acos acos /**< arc cosine */
#define c_cos  cos  /**< cosine */
#endif /* DFLOAT */

/** @} */

# ifdef __cplusplus
}
# endif 

#endif /* ifndef GLOBAL_OPTS_H */