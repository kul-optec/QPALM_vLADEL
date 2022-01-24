/**
 * @file nonconvex.h
 * @author Ben Hermans
 * @brief Routines to deal with nonconvex QPs.
 * @details The functions in this file serve to set up QPALM for a nonconvex QP. The main routine in 
 * this file computes the minimum eigenvalue of a square matrix, based on lobpcg \cite knyazev2001toward. Furthermore, 
 * some setting updates are performed. In addition, the spectrum of a matrix can be upper bounded using 
 * Gershgorin's circle theorem, which is used in the gamma_boost routine in iteration.c.
 */

#ifndef NONCONVEX_H
#define NONCONVEX_H

# ifdef __cplusplus
extern "C" {
# endif 

#include "types.h"

/**
 * Calculate the Gershgorin upper bound for the eigenvalues of a symmetric matrix.
 * 
 * This routine uses the Gershgorin circle theorem to compute an upper bound on the eigenvalues of 
 * a matrix.
 * 
 * @note M is assumed to be symmetric
 * 
 * @param M Matrix
 * @param center Vector of size M->ncol to hold the values of the centers of the discs
 * @param radius Vector of size M->ncol to hold the values of the radii of the discs
 * @return Upper bound on the eigenvalues of M
 */
c_float gershgorin_max( solver_sparse   *M, 
                        c_float         *center, 
                        c_float         *radius);

/**
 * Set the proximal parameters for nonconvex QPs.
 * 
 * QPALM can deal with nonconvex QPs, by setting the initial and maximal proximal penalty small enough 
 * (smaller than @f$ \frac{1}{|\lambda_\textrm{min}|} @f$). This ensures positive definiteness of 
 * @f$ Q + \frac{1}{\gamma}I @f$ during the iterations. The minimum eigenvalue is computed using lobpcg.
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void set_settings_nonconvex(QPALMWorkspace  *work, 
                            solver_common   *c);


# ifdef __cplusplus
}
# endif /* ifdef __cplusplus */

#endif /*NONCONVEX_H*/