/**
 * @file iteration.h
 * @author Ben Hermans
 * @brief QPALM main solver routines.
 * @details This file contains the functions that make up the qpalm algorithm (the functions that qpalm_solve will use). 
 * These include the computation of the residuals at the start of the iteration, 
 * the update of the primal variables in an inner iteration, 
 * the update of the penalty factors and dual variables in an outer iteration,
 * the computation of the primal and dual objective values, etc.
 */

#ifndef ITERATION_H
#define ITERATION_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "types.h"
#include "global_opts.h"

/**
 * Compute the residuals (in vector form)
 * 
 * This routine is called at the start of every QPALM iteration. It computes the current residual vectors,
 * which are required by the termination routines and by the other iteration routines.
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void compute_residuals( QPALMWorkspace  *work, 
                        solver_common   *c);

/**
 * Initialize penalty factors from initial x.
 * 
 * The formula used here can be found in \cite birgin2014practical.  
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void initialize_sigma(  QPALMWorkspace  *work, 
                        solver_common   *c);

/**
 * Update the penalty factors. 
 * 
 * Constraints that are active are penalized in proportion to their constraint violation.
 * If the number of changed penalty parameters is low, and the proximal penalty need not be further updated,
 * then the factor LD is updated using a low-rank update based on the changed penalty factors.
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void update_sigma(  QPALMWorkspace  *work, 
                    solver_common   *c);

/**
 * Update the proximal penalty. 
 * 
 * The proximal penalty parameter is increased by a constant factor at every outer update,
 * until a certain maximum value is reached.
 * @param work Workspace
 */
void update_gamma(QPALMWorkspace* work);

/**
 * Maximize the proximal penalty. 
 * 
 * The proximal penalty parameter can be increased when the primal residual is low and the number
 * of active constraints no longer changes. Increasing the proximal penalty will significantly speed
 * up the convergence of the dual residual in that case. First, the maximum allowed value is computed,
 * based on the fact that @f$Q+A^T \Sigma A + \frac{1}{\gamma}I@f$ should be sufficiently positive 
 * definite for the factorization routines.
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void boost_gamma(   QPALMWorkspace  *work, 
                    solver_common   *c);

void update_or_boost_gamma( QPALMWorkspace  *work, 
                            solver_common   *c, 
                            c_int           iter_out);

void update_proximal_point_and_penalty(QPALMWorkspace *work, solver_common *c, c_int iter_out, c_float *eps_k_abs, c_float *eps_k_rel);

void update_dual_iterate_and_parameters(QPALMWorkspace *work, solver_common *c, c_int iter_out, c_float *eps_k_abs, c_float *eps_k_rel);


/**
 * Update the primal iterate.
 * 
 * This function calls the functions that compute the newton direction and stepsize from exact linesearch,
 * and applies the update.
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void update_primal_iterate( QPALMWorkspace *work, 
                            solver_common   *c);

/**
 * Compute the (unscaled) primal objective value at the current iterate.
 * @return The value of the primal objective at the current iterate.
 * @param work Workspace
 */ 
c_float compute_objective(QPALMWorkspace *work);

/**
 * Compute the (unscaled) dual objective value at the current iterate.
 * @return The value of the dual objective at the current iterate.
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */ 
c_float compute_dual_objective( QPALMWorkspace *work, 
                                solver_common   *c);


# ifdef __cplusplus
}
# endif 

#endif