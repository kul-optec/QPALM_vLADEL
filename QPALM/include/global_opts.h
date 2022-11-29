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

#ifdef WIN32
#    ifdef __GNUC__
#        ifdef QPALM_EXPORTS
#            define QPALM_EXPORT __attribute__((dllexport))
#        elif QPALM_IMPORTS
#            define QPALM_EXPORT __attribute__((dllimport))
#        else
#            define QPALM_EXPORT
#        endif
#    else /* __GNUC__ */
#        ifdef QPALM_EXPORTS
#            define QPALM_EXPORT __declspec(dllexport)
#        elif QPALM_IMPORTS
#            define QPALM_EXPORT __declspec(dllimport)
#        else
#            define QPALM_EXPORT
#        endif
#    endif /* __GNUC__ */
#else /* WIN32 */
#    define QPALM_EXPORT __attribute__((visibility("default")))
#endif /* WIN32 */

# ifdef __cplusplus
extern "C" {
# endif 

#include "ladel.h"
typedef ladel_double  c_float; /**< type for floating point numbers */
typedef ladel_int     c_int; /**< type for integer numbers */

/**
 * @name Custom memory allocation (e.g. matlab/python)
 * @{
 */

QPALM_EXPORT void *qpalm_calloc(size_t num, size_t size);
QPALM_EXPORT void *qpalm_malloc(size_t size);
QPALM_EXPORT void* qpalm_realloc(void *ptr, size_t size);
QPALM_EXPORT void qpalm_free(void *ptr);

/** Set the `calloc` function used by QPALM. */
QPALM_EXPORT calloc_sig *qpalm_set_alloc_config_calloc(calloc_sig *calloc);
/** Set the `malloc` function used by QPALM. */
QPALM_EXPORT malloc_sig *qpalm_set_alloc_config_malloc(malloc_sig *malloc);
/** Set the `realloc` function used by QPALM. */
QPALM_EXPORT realloc_sig *qpalm_set_alloc_config_realloc(realloc_sig *realloc);
/** Set the `free` function used by QPALM. */
QPALM_EXPORT free_sig *qpalm_set_alloc_config_free(free_sig *free);

/** Set the `printf` function used by QPALM. */
QPALM_EXPORT printf_sig *qpalm_set_print_config_printf(printf_sig *printf);

/**
 * @}
 */


/* QPALM_PRINTING */
# ifdef QPALM_PRINTING

// Print macro
#  define qpalm_print ladel_print

// Print error macro
#  ifdef __GNUC__
#    define qpalm_eprint(...) __extension__ ({ qpalm_print("ERROR in %s: ", __FUNCTION__); qpalm_print(__VA_ARGS__); qpalm_print("\n"); })
#  else
#    define qpalm_eprint(...) do { qpalm_print("ERROR in %s: ", __FUNCTION__); qpalm_print(__VA_ARGS__); qpalm_print("\n"); } while (0)
#  endif

# endif /* ifdef QPALM_PRINTING */


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