/**
 * @file solver_interface.h
 * @author Ben Hermans
 * @brief Interface and wrapper to matrix/factorization (ladel) functions
 * @details This file includes all calls to ladel functions apart from scaling in scaling.c and memory
 * allocation/deallocation in the main functions in qpalm.c. It includes all matrix operations, such as
 * matrix vector products, row- and columnwise norms, cholesky factorizations, factorization updates and
 * solving the linear system. 
 * 
 */

#ifndef SOLVER_INTERFACE_H
#define SOLVER_INTERFACE_H

# ifdef __cplusplus
extern "C" {
# endif 

#include "global_opts.h"
#include "constants.h"
#include "types.h"

/** 
 * Matrix-vector multiplication.
 *    
 * @f$y  =  A*x@f$
 * 
 * @param A Sparse matrix
 * @param x Dense input vector
 * @param y Dense output vector
 * @param c Solver environment 
 */
void mat_vec(solver_sparse *A,
             solver_dense  *x,
             solver_dense  *y,
             solver_common *c);
/** 
 * Matrix-transpose-vector multiplication.
 *    
 * @f$y  =  A^T*x@f$ 
 * 
 * @param A Sparse matrix
 * @param x Dense input vector
 * @param y Dense output vector
 * @param c Solver environment 
 */
void mat_tpose_vec(solver_sparse *A,
                   solver_dense  *x,
                   solver_dense  *y,
                   solver_common *c);

/**
 * Choose the linear systems solver method based on the problem data sizes.
 * 
 * This chooses between forming and solving the KKT system or the SCHUR complement.
 * The resulting method is in work->solver->factorization_method.
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void qpalm_set_factorization_method(QPALMWorkspace  *work, 
                                    solver_common   *c);

#include "ladel.h"

#define mat_inf_norm_cols ladel_infinity_norm_columns
#define mat_inf_norm_rows ladel_infinity_norm_rows

/**
 * Form the KKT system 
 * @f$\begin{bmatrix}
 * Q & A^T \\
 * A & -\Sigma^{-1}
 * \end{bmatrix} @f$. The result is in work->solver->kkt.
 * 
 * @note Only the rows of A corresponding to active constraints are included in the system above.
 * This routine also saves some structures that are useful in updating the kkt system later.
 * 
 * @param work  Workspace
 */
void qpalm_form_kkt(QPALMWorkspace *work);

/**
 * Reform the KKT system (i.e. delete constraints which are no longer active and add those that are now active). 
 * 
 * @param work  Workspace
 */
void qpalm_reform_kkt(QPALMWorkspace *work);

/**
 * Perform a factorization update for the entering constraints.
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void kkt_update_entering_constraints(   QPALMWorkspace *work, 
                                        solver_common   *c);

/**
 * Perform a factorization update for the leaving constraints.
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void kkt_update_leaving_constraints(QPALMWorkspace  *work, 
                                    solver_common   *c);

/**
 * Solve the KKT system
 * @f$\begin{bmatrix}
 * Q + \frac{1}{\gamma}I & A^T \\
 * A & -\Sigma^{-1}
 * \end{bmatrix} \begin{bmatrix}
 * d \\
 * -\lambda
 * \end{bmatrix} = \begin{bmatrix}
 * -\nabla \varphi \\
 * 0
 * \end{bmatrix}
 * @f$
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment
 */
void kkt_solve( QPALMWorkspace  *work, 
                solver_common   *c);


/**
 * Calculate @f$LDL^T@f$ factorization of a matrix @f$M@f$.
 * 
 * If work->settings->proximal = true, use @f$M+\frac{1}{\gamma}*I@f$ instead.
 * 
 * @param M Matrix to be factorized
 * @param work Workspace
 * @param c Solver environment
 */
void ldlchol(solver_sparse *M, 
             QPALMWorkspace *work,
             solver_common *c);


/**
 * Calculate @f$LDL^T@f$ factorization of @f$Q+A{(a,:)}^T*\Sigma{(a,a)}*A{(a,:)}@f$, with @f$\Sigma=diag(\sigma)@f$ and @f$a@f$ the set of active constraints.
 * 
 * If work->settings->proximal = true, use @f$Q+\frac{1}{\gamma}*I+A{(a,:)}^T*\Sigma{(a,a)}*A{(a,:)}@f$ instead.
 * 
 * @param work Workspace
 * @param c Solver environment
 */
void ldlcholQAtsigmaA(QPALMWorkspace *work,
                      solver_common *c);

/**
 * Update the @f$LDL^T@f$ factorization given a set of entering constraints.
 * 
 * The index set of entering constraints is assumed to be set in work->solver->enter.
 * 
 * @param work Workspace
 * @param c Solver environment
 */
void ldlupdate_entering_constraints(QPALMWorkspace *work,
                                    solver_common *c);

/**
 * Downdate the @f$LDL^T@f$ factorization given a set of leaving constraints.
 * 
 * The index set of leaving constraints is assumed to be set in work->solver->leave.
 * 
 * @param work Workspace
 * @param c Solver environment
 */
void ldldowndate_leaving_constraints(QPALMWorkspace *work,
                                     solver_common *c);

/**
 * Update the @f$LDL^T@f$ factorization given a set of indexes where @f$sigma@f$ has been updated.
 * 
 * The index set of changed @f$sigma@f$ is assumed to be set in work->solver->enter.
 * 
 * @param work Workspace
 * @param c Solver environment
 */
void ldlupdate_sigma_changed(QPALMWorkspace *work,
                             solver_common *c);


/**
 * Solve the linear system @f$LDL^T*d = -\nabla \varphi@f$.
 * 
 * @param work Workspace
 * @param c Solver environment
 */
void ldlsolveLD_neg_dphi(QPALMWorkspace *work,
                         solver_common *c);


# ifdef __cplusplus
}
# endif 

#endif // ifndef CHOLMOD_INTERFACE_H 