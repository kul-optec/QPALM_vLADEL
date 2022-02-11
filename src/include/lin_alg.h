/**
 * @file lin_alg.h
 * @author Ben Hermans
 * @brief Linear algebra with vectors.
 * @details Common operations, such as vector products, infinity norm, elementwise 
 * add/product/division/max etc. are included in this file.
 */

#ifndef LIN_ALG_H
# define LIN_ALG_H


# ifdef __cplusplus
extern "C" {
# endif 

# include "types.h"
#include "solver_interface.h"

/**
 * @name Vector functions
 * @{
 */

/**
 * Copy vector a into output.
 * @warning This function uses malloc.
 * @param a Vector
 * @param n Vector length
 * @return Copy of a
 */
c_float* vec_copy(const c_float *a,
                  size_t         n);

/** 
 * Copy vector a into preallocated vector b.
 * 
 * @param a Input vector
 * @param b Output vector
 * @param n Vector length
 */
void prea_vec_copy(const c_float *a,
                   c_float       *b,
                   size_t         n);

/**
 * Copy integer vector a into preallocated vector b.
 * 
 * @param a Input vector
 * @param b Output vector
 * @param n Vector length
 */
void prea_int_vec_copy(const c_int *a,
                       c_int       *b,
                       size_t       n);

/**
 * Fill float vector with a scalar value.
 * 
 * @param a Vector
 * @param sc Value
 * @param n Vector length
 */
void vec_set_scalar(c_float *a,
                    c_float  sc,
                    size_t   n);

/**
 * Fill int vector with a scalar value.
 * 
 * @param a Vector
 * @param sc Value
 * @param n Vector length
 */
void vec_set_scalar_int(c_int *a, 
                        c_int  sc, 
                        size_t n);

/**
 * Mulitply vector with a constant scale factor.
 * 
 * @param a Vector
 * @param sc Value
 * @param n Vector length 
 */
void vec_self_mult_scalar(  c_float *a,
                            c_float sc,
                            size_t  n);

/**
 * Mulitply vector with a constant scale factor and store in a different vector.
 * 
 * @param a Input vector
 * @param sc Value
 * @param b Output vector
 * @param n Vector length 
 */
void vec_mult_scalar(const c_float  *a,
                     c_float        sc,
                     c_float        *b,
                     size_t         n);

/** 
 * Inner product between two vectors, @f$a^T \cdot b@f$. 
 * 
 * @param a Vector
 * @param b Vector
 * @param n Vector length
 * @return Result of the inner product
 */
c_float vec_prod(const c_float *a,
                 const c_float *b,
                 size_t         n);

/** 
 * 2-norm of a vector, @f$\|a\|_2@f$. 
 * 
 * @param a Vector
 * @param n Vector length
 * @return 2-norm of a
 */
c_float vec_norm_two(   const c_float   *a, 
                        size_t          n);

/**
 * Infinity norm of a vector, @f$\|a\|_\infty@f$
 * 
 * @param a Vector
 * @param n Vector length
 * @return Infinity norm of a
 */
c_float vec_norm_inf(const c_float *a,
                     size_t         n);

/**
 * Scaled addition of one vector to another vector, @f$c_i = a_i + sc\cdot b_i@f$ 
 * 
 * @param a Input vector
 * @param b Input vector
 * @param c Output vector
 * @param sc Scaling value
 * @param n Vector length
 * */
void vec_add_scaled(const c_float *a,
                    const c_float *b,
                    c_float       *c,
                    c_float        sc,
                    size_t         n);

/**
 * Scaled addition of one vector to another vector, both being scaled, @f$a_i = sc1\cdot a_i + sc2\cdot b_i@f$ 
 * 
 * @param a Input and Output vector
 * @param b Input vector
 * @param sc1 Scaling value for a
 * @param sc2 Scaling value for b
 * @param n Vector length
 * */
void vec_mult_add_scaled(   c_float         *a, 
                            const c_float   *b, 
                            c_float         sc1, 
                            c_float         sc2, 
                            size_t          n);

/**
 * Elementwise reciprocal @f$b_i = 1/a_i@f$.
 * 
 * This function is used in scaling.
 * 
 * @param a Input vector
 * @param b Output vector
 * @param n Vector length
 */
void vec_ew_recipr(const c_float *a,
                   c_float       *b,
                   size_t         n);

/** 
 * Elementwise maximum between vectors, @f$c_i = \textrm{max}(a_i, b_i)@f$.
 * 
 * @param a Input vector
 * @param b Input vector
 * @param c Output vector
 * @param n Vector length
 */
void vec_ew_max_vec(const c_float *a,
                    const c_float *b,
                    c_float       *c,
                    size_t         n);

/** 
 * Elementwise minimum between vectors, @f$c_i = \textrm{min}(a_i, b_i)@f$.
 * 
 * @param a Input vector
 * @param b Input vector
 * @param c Output vector
 * @param n Vector length
 */
void vec_ew_min_vec(const c_float *a,
                    const c_float *b,
                    c_float       *c,
                    size_t         n);

/** 
 * Elementwise mid between vectors, @f$c_i = \textrm{max}(b_{\textrm{min},i}, \textrm{min}(a_i, b_{\textrm{max},i}))@f$.
 * 
 * @param a Input vector
 * @param bmin Lower bounds
 * @param bmax Upper bounds
 * @param c Output vector
 * @param n Vector length
 */
void vec_ew_mid_vec(const c_float *a,
                    const c_float *bmin,
                    const c_float *bmax,
                    c_float       *c,
                    size_t         n);

/**
 * Elementwise product, @f$c_i = a_i\cdot b_i@f$.
 * 
 * @param a Input vector
 * @param b Input vector
 * @param c Output vector
 * @param n Vector length
 */
void vec_ew_prod(const c_float *a,
                 const c_float *b,
                 c_float       *c,
                 size_t         n);

/**
 * Elementwise division, @f$c_i = a_i/b_i@f$.
 * 
 * @param a Input vector
 * @param b Input vector
 * @param c Output vector
 * @param n Vector length
 */
void vec_ew_div(const c_float *a,
                const c_float *b,
                c_float       *c,
                size_t         n);

/** 
 * Elementwise square root, @f$b_i = \sqrt{a_i}@f$ 
 * 
 * @param a Input vector
 * @param b Output vector
 * @param n Vector length
 */
void vec_ew_sqrt(const c_float *a,
                 c_float       *b,
                 size_t         n);


/**
 * @}
 */

/* MATRIX FUNCTIONS ----------------------------------------------------------*/

/* Moved to solver_interface.c*/


# ifdef __cplusplus
}
# endif 

#endif // ifndef LIN_ALG_H