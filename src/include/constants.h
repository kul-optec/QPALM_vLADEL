/**
 * @file constants.h
 * @author Ben Hermans
 * @brief Constants used in QPALM
 * @details This file contains the constants that are used as default settings and to set the solver status.
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif 

/**
 * @name Booleans
 * @{
 */
#define TRUE 1
#define FALSE 0
/**
 * @}
 */


/** 
 * @name Solver status
 * @{
 */

#define QPALM_SOLVED (1)               /**< status to indicate the problem is solved to optimality given the specified tolerances */
#define QPALM_DUAL_TERMINATED (2)      /**< status to indicate the problem has a dual objective that is higher than the specified bound */
#define QPALM_MAX_ITER_REACHED (-2)    /**< status to indicate termination due to reaching the maximum number of iterations  */
#define QPALM_PRIMAL_INFEASIBLE (-3)   /**< status to indicate the problem is primal infeasible  */
#define QPALM_DUAL_INFEASIBLE (-4)     /**< status to indicate the problem is dual infeasible  */
#define QPALM_TIME_LIMIT_REACHED (-5)  /**< status to indicate the problem's runtime has exceeded the specified time limit */
#define QPALM_UNSOLVED (-10)         /**< status to indicate the problem is unsolved. Only setup function has been called */
#define QPALM_ERROR (0)              /**< status to indicate an error has occured (this error should automatically be printed) */

/**
 * @}
 */

/** 
 * @name Solver parameters and settings
 * @{
 */

/**********************************
* Solver Parameters and Settings *
**********************************/

#ifndef QPALM_NULL
    #define QPALM_NULL 0 /**< NULL, if something goes wrong during setup, the workspace pointer is set to this */
#endif /* ifndef QPALM_NULL */

#ifndef QPALM_NAN
    #define QPALM_NAN ((c_float)0x7fc00000UL)  /**< not a number, used for the solution if the problem is primal or dual infeasible */
#endif /* ifndef QPALM_NAN */

#ifndef QPALM_INFTY
    #define QPALM_INFTY ((c_float)1e20)  /**< infinity, used to indicate one-sided constraints */
#endif /* ifndef QPALM_INFTY */


#define MAX_ITER (10000)       /**< default maximum number of iterations */
#define INNER_MAX_ITER (100)   /**< default maximum number of iterations per subproblem */
#define EPS_ABS (1e-4)         /**< default absolute convergence tolerance */
#define EPS_REL (1e-4)         /**< default relative convergence tolerance */
#define EPS_ABS_IN (1)         /**< default intermediate absolute convergence tolerance */
#define EPS_REL_IN (1)         /**< default intermediate relative convergence tolerance */
#define RHO (0.1)              /**< default tolerance scaling factor */
#define EPS_PRIM_INF (1e-5)    /**< default primal infeasibility tolerance */
#define EPS_DUAL_INF (1e-5)    /**< default dual infeasibility tolerance */
#define THETA (0.25)           /**< default penalty update criterion parameter */
#define DELTA (100)            /**< default penalty update factor */
#define SIGMA_MAX (1e9)        /**< default penalty cap */
#define SIGMA_INIT (2e1)       /**< default initial penalty parameter (guideline) */
#define PROXIMAL (TRUE)        /**< default use of proximal method of multipliers */
#define GAMMA_INIT (1E7)       /**< default initial proximal penalty parameter */
#define GAMMA_UPD (10)         /**< default proximal penalty update factor */
#define GAMMA_MAX (1E7)        /**< default proximal penalty cap */

#define SCALING (10)           /**< default number of scaling iterations */
#define MIN_SCALING (1e-12)    /**< minimum scaling value *////< Minimum scaling value
#define MAX_SCALING (1e+04)    /**< maximum scaling value *////< Maximum scaling value

#define NONCONVEX (FALSE)       /**< default use of nonconvex adjustments */
#define WARM_START (FALSE)     /**< default warm start setting */
#define VERBOSE (TRUE)         /**< default write out progress setting */
#define PRINT_ITER (1)         /**< default frequency of printing */

#define RESET_NEWTON_ITER (10000) /**< default frequency of performing a full Cholesky factorization */

#define ENABLE_DUAL_TERMINATION (FALSE) /**< enable termination after dual objective > something (useful in branch and bound) */
#define DUAL_OBJECTIVE_LIMIT (QPALM_INFTY) /**< termination value for the dual objective (useful in branch and bound) */
#define TIME_LIMIT (QPALM_INFTY) /**< time limit after which the solver aborts */

#define MAX_RANK_UPDATE 160 /**< maximum rank for the sparse factorization update */
#define MAX_RANK_UPDATE_FRACTION 0.1 /**< maximum rank (relative to n+m) for the factorization update */

#define RELATIVE_REFINEMENT_TOLERANCE 1e-10 /**< relative tolerance on the residual for linear systems solving */
#define ABSOLUTE_REFINEMENT_TOLERANCE 1e-12 /**< absolute tolerance on the residual for linear systems solving */
#define MAX_REFINEMENT_ITERATIONS     3     /**< maximum number of refinement iterations */  

/* Options for settings->factorization_method */
#define FACTORIZE_KKT 0 /**< factorize the kkt system */
#define FACTORIZE_SCHUR 1 /**< factorize the Schur complement */
#define FACTORIZE_KKT_OR_SCHUR 2 /**< select automatically between kkt system and schur complement */

#define FACTORIZATION_METHOD FACTORIZE_KKT_OR_SCHUR /**< default method for solving the linear system */

#include "ladel.h"
#define ORDERING AMD /**< ordering in the factorization */

/**
 * @}
 */


#ifdef __cplusplus
}
#endif 

#endif // ifndef CONSTANTS_H