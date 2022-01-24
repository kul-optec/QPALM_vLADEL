/**
 * @file linesearch.h
 * @author Ben Hermans
 * @brief Routines to perform exact linesearch.
 * @details Once the direction is found using the semismooth Newton method, the functions in this file can 
 * be called to calculate a stepsize with exact linesearch.
 */

#ifndef LINESEARCH_H
#define LINESEARCH_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "types.h"
#include "constants.h"


/**
 * Execute exact linesearch (using qsort)
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment
 * @return      tau  Step size
 */
c_float exact_linesearch(QPALMWorkspace *work, solver_common *c);

/**
 * Helper function to copy vector a in array b (with indices)
 * 
 * @param a Vector
 * @param b Array (vector with added indices)
 * @param n Vector length
 */
void vec_array_copy(c_float       *a, 
                    array_element *b, 
                    size_t         n);

/**
 * Select subsequence based on a set of indices, @f$b = a(L)@f$ 
 * 
 * @param a Input array
 * @param b Outpur array
 * @param L Index set
 * @param n Length of a
 */
void select_subsequence(const array_element *a, 
                        array_element       *b,
                        const c_int         *L,
                        size_t               n);

/**
 * Inner product over index set, @f$a(L)^T\cdot b(L)@f$ 
 * 
 * @param a Vector
 * @param b Vector
 * @param L Index set
 * @param n Vector length 
 */
c_float vec_prod_ind(const c_float *a,
                     const c_float *b,
                     const c_int   *L,
                     size_t         n);

/**
 * Helper function for qsort
 * 
 * Carries out comparison between two array_elements
 * @param a Pointer to array element
 * @param b Pointer to array element
 * @return a.x > b.x
 */
int compare (const void * a, 
             const void * b);

# ifdef __cplusplus
}
# endif 

#endif