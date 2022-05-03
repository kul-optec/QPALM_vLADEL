/**
 * @file types.h
 * @author Ben Hermans
 * @brief Internal data structures used in QPALM.
 * @details See data structures tab.
 */

#ifndef QPALM_TYPES_H
# define QPALM_TYPES_H

# ifdef __cplusplus
extern "C" {
# endif 

#include "global_opts.h"

#include "ladel.h"
typedef ladel_work            solver_common;
typedef ladel_sparse_matrix   solver_sparse;
typedef ladel_double          solver_dense;
typedef ladel_factor          solver_factor;
typedef ladel_symbolics       solver_symbolics;

/**
 * Array to sort in linesearch
 */
typedef struct array_element  {
  c_float x; ///< value of the element
  size_t  i; ///< index
} array_element;


/// @addtogroup solver-grp
/// @{

/******************
* Internal types *
******************/

/**
 * Solution structure
 */
typedef struct {
  c_float *x; ///< primal solution
  c_float *y; ///< dual solution
} QPALMSolution;

/// @}

/**
 * QPALM Timer for statistics
 */
typedef struct QPALM_TIMER QPALMTimer;

/// @addtogroup solver-grp
/// @{

/**
 * Problem scaling matrices stored as vectors
 */
typedef struct {
  c_float *D;    ///< primal variable scaling
  c_float *Dinv; ///< primal variable rescaling
  c_float *E;    ///< dual variable scaling
  c_float *Einv; ///< dual variable rescaling
  c_float  c;    ///< objective scaling
  c_float  cinv; ///< objective rescaling
} QPALMScaling;


/**
 * Solver return information
 */
typedef struct {
  c_int   iter;           ///< number of iterations taken
  c_int   iter_out;       ///< number of outer iterations (i.e. dual updates)
  char    status[32];     ///< status string, e.g. 'solved'
  c_int   status_val;     ///< status as c_int, defined in constants.h

  c_float pri_res_norm;   ///< norm of primal residual
  c_float dua_res_norm;   ///< norm of dual residual
  c_float dua2_res_norm;  ///< norm of intermediate dual residual (minus proximal term)

  c_float objective;      ///< objective function value
  c_float dual_objective; ///< dual objective function value (= NaN if enable_dual_termination is false)

  #ifdef QPALM_TIMING
  c_float setup_time;    ///< time taken for setup phase (seconds)
  c_float solve_time;    ///< time taken for solve phase (seconds)
  c_float run_time;      ///< total time (seconds)
  #endif

} QPALMInfo;

/**********************************
* Main structures and Data Types *
**********************************/

/**
 * Data structure
 */
typedef struct {
  size_t          n;    ///< number of variables n
  size_t          m;    ///< number of constraints m
  solver_sparse  *Q;    ///< sparse quadratic part of the cost Q (size n x n)           
  solver_sparse  *A;    ///< sparse linear constraints matrix A (size m x n)
  c_float        *q;    ///< dense array for linear part of cost function (size n)
  c_float         c;    ///< constant part of cost 
  c_float        *bmin; ///< dense array for lower bounds (size m)
  c_float        *bmax; ///< dense array for upper bounds (size m)
} QPALMData;


/**
 * Settings struct
 */
typedef struct {
  c_int   max_iter;                 ///< maximum number of iterations @details @note Assumption: @f$>0@f$
  c_int   inner_max_iter;           ///< maximum number of iterations per subproblem @details @note Assumption: @f$>0@f$
  c_float eps_abs;                  ///< absolute convergence tolerance @details @note Assumption: @f$>=0@f$, either eps_abs or eps_rel must be @f$>0@f$
  c_float eps_rel;                  ///< relative convergence tolerance @details @note Assumption: @f$>=0@f$, either eps_abs or eps_rel must be @f$>0@f$
  c_float eps_abs_in;               ///< intermediate absolute convergence tolerance @details @note Assumption: @f$>=0@f$, either eps_abs_in or eps_rel_in must be @f$>0@f$
  c_float eps_rel_in;               ///< intermediate relative convergence tolerance @details @note Assumption: @f$>=0@f$, either eps_abs_in or eps_rel_in must be @f$>0@f$
  c_float rho;                      ///< tolerance scaling factor @details @note Assumption: @f$0<\rho<1@f$
  c_float eps_prim_inf;             ///< primal infeasibility tolerance @details @note Assumption: @f$>=0@f$
  c_float eps_dual_inf;             ///< dual infeasibility tolerance @details @note Assumption: @f$>=0@f$
  c_float theta;                    ///< penalty update criterion parameter @details @note Assumption: @f$<=1@f$
  c_float delta;                    ///< penalty update factor @details @note Assumption: @f$>1@f$
  c_float sigma_max;                ///< penalty factor cap @details @note Assumption: @f$>0@f$ 
  c_float sigma_init;               ///< initial penalty parameter (guideline) @note Assumption: @f$>0@f$ 
  c_int   proximal;                 ///< boolean, use proximal method of multipliers or not @details @note Assumption: @f$\in \{0,1\}@f$ 
  c_float gamma_init;               ///< initial proximal penalty parameter @details @note Assumption: @f$>0@f$
  c_float gamma_upd;                ///< proximal penalty update factor @details @note Assumption: @f$>=1@f$
  c_float gamma_max;                ///< proximal penalty parameter cap @details @note Assumption: @f$>=\gamma_\textrm{init}@f$
  c_int   scaling;                  ///< scaling iterations, if 0 then scaling is disabled @details @note Assumption: @f$>=0@f$
  c_int   nonconvex;                ///< boolean, indicates whether the QP is nonconvex @details @note Assumption: @f$\in \{0,1\}@f$ 
  c_int   verbose;                  ///< boolean, write out progress @details @note Assumption:@f$\in \{0,1\}@f$
  c_int   print_iter;               ///< frequency of printing @details @note Assumption: @f$>0@f$
  c_int   warm_start;               ///< boolean, warm start @details @note Assumption: @f$\in \{0,1\}@f$
  c_int   reset_newton_iter;        ///< frequency of performing a complete Cholesky factorization @details @note Assumption: @f$>0@f$
  c_int   enable_dual_termination;  ///< boolean, enable termination based on dual objective (useful in branch and bound) @details @note Assumption: @f$\in \{0,1\}@f$
  c_float dual_objective_limit;     ///< termination value for the dual objective (useful in branch and bound) @details @note Assumption: none
  c_float time_limit;               ///< time limit @details @note Assumption: @f$>0@f$
  c_int   ordering;                 ///< ordering method for factorization
  c_int   factorization_method;     ///< factorize KKT or Schur complement
  c_int   max_rank_update;          ///< maximum rank for the sparse factorization update
  c_float max_rank_update_fraction; ///< maximum rank (relative to n+m) for the factorization update
} QPALMSettings;

/// @}

/**
 * Variables for linear system solving 
 */
typedef struct {
  c_int factorization_method;   ///< factorize KKT or Schur complement
  solver_sparse *kkt;           ///< KKT matrix
  solver_sparse *kkt_full;      ///< KKT matrix if all constraints would be active
  solver_sparse *At;            ///< A'
  c_int *first_row_A;           ///< row index of the first element in each column of A'
  c_float *first_elem_A;        ///< value of the first element in each column of A'
  solver_factor *LD;             ///< LD factor (part of LDL' factorization)
  solver_symbolics *sym;         ///< symbolics for kkt (only used in LADEL)
  solver_factor *LD_Q;           ///< LD factor of Q (useful in computing dual objective)
  solver_symbolics *sym_Q;       ///< symbolics for Q (only used in LADEL)
  solver_dense *E_temp;          ///< temporary constraints scaling vectors
  solver_dense *D_temp;          ///< temporary primal variable scaling vectors
  solver_dense *neg_dphi;        ///< -gradient of the Lagrangian
  solver_dense *rhs_kkt;         ///< [-dphi; zeros(m,1)]
  solver_dense *sol_kkt;         ///< sol_kkt = kkt \\ rhs_kkt
  solver_dense *d;               ///< primal update step
  solver_dense *Ad;              ///< A * d
  solver_dense *Qd;              ///< Q * d
  solver_dense *yh;              ///< candidate dual update
  solver_dense *Atyh;            ///< A' * yh
  c_int first_factorization;      ///< boolean, indicate we have not factorized previously
  c_int reset_newton;             ///< boolean, after sigma is updated perform a new factorization
  c_int *active_constraints;      ///< index set of active constraints
  c_int *active_constraints_old;  ///< index set of active constraints in the previous iteration
  c_int nb_active_constraints;    ///< number of active constraints
  c_int *enter;                   ///< index set of entering constraints
  c_int nb_enter;                 ///< number of entering constraints
  c_int *leave;                   ///< index set of leaving constraints
  c_int nb_leave;                 ///< number of leaving constraints
  solver_dense *At_scale;        ///< running vector of sqrt(sigma), used to scale At_sqrt_sigma
  solver_sparse *At_sqrt_sigma;  ///< A' * sqrt(sigma)
} QPALMSolver;


/**
 * QPALM Workspace
 * 
 * The workspace is the main data structure and is given as a pointer to (almost) all QPALM functions.
 * It contains pointers to the settings, the data, return info, solution variables and intermediate 
 * workspace variables.
 */
typedef struct {
  QPALMData *data; ///< problem data to work on (possibly scaled)

  /**
   * @name Iterates
   * @{
   */
  c_float *x;        ///< primal iterate
  c_float *y;        ///< dual iterate
  c_float *Ax;       ///< scaled A * x
  c_float *Qx;       ///< scaled Q * x
  c_float *Aty;      ///< A' * y (useful for saving one mat_tpose_vec)
  c_float *x_prev;   ///< previous primal iterate
  c_int initialized; ///< flag whether the iterates were initialized or not
  /** @} */

  /**
   * @name Workspace variables
   * @{ 
   */
  c_float *temp_m;        ///< placeholder for vector of size m
  c_float *temp_n;        ///< placeholder for vector of size n
  c_float *sigma;         ///< penalty vector
  c_float *sigma_inv;     ///< 1./sigma
  c_float sqrt_sigma_max;  ///< sqrt(sigma_max)
  c_int nb_sigma_changed; ///< number of sigma-components that changed in an outer iteration (relevant for factorization update)
  c_float gamma;          ///< proximal penalty factor
  c_int gamma_maxed;      ///< flag to indicate whether gamma has been maximized when the primal residual was low
  c_float *Axys;          ///< Ax + y./sigma
  c_float *z;             ///< projection of Axys onto the constraint set [bmin, bmax]
  c_float *pri_res;       ///< primal residual
  c_float *pri_res_in;    ///< intermediate primal residual
  c_float *yh;            ///< candidate dual update
  c_float *Atyh;          ///< A' * yh
  c_float *df;            ///< gradient of the primal objective (+proximal term)
  c_float *x0;            ///< record of the primal iterate during the last dual update
  c_float *xx0;           ///< x - x0
  c_float *dphi;          ///< gradient of the Lagrangian
  c_float *neg_dphi;      ///< -dphi, required as the rhs in SCHUR
  c_float *dphi_prev;     ///< previous gradient of the Lagrangian
  c_float *d;             ///< primal update step

  /** @} */

  /**
   * @name Linesearch variables
   * @{
   */
  c_float tau;          ///< stepsize
  c_float *Qd;          ///< Q * d
  c_float *Ad;          ///< A * d
  c_float *sqrt_sigma;  ///< elementwise sqrt(sigma)
  c_float sqrt_delta;   ///< sqrt(penalty update factor)
  c_float eta;          ///< linesearch parameter
  c_float beta;         ///< linesearch parameter
  c_float *delta;       ///< linesearch parameter
  c_float *alpha;       ///< linesearch parameter
  c_float *temp_2m;     ///< placeholder for vector of size 2m
  c_float *delta2;      ///< delta .* delta
  c_float *delta_alpha; ///< delta .* alpha
  array_element *s;     ///< alpha ./ delta
  c_int   *index_L;     ///< index set L (where s>0)
  c_int   *index_P;     ///< index set P (where delta>0)
  c_int   *index_J;     ///< index set J (L xor P)

  /** @} */

  /**
   * @name Termination criteria variables
   * @{ 
   */
  c_float eps_pri;    ///< primal tolerance
  c_float eps_dua;    ///< dual tolerance
  c_float eps_dua_in; ///< intermediate dual tolerance
  c_float eps_abs_in; ///< intermediate absolute tolerance
  c_float eps_rel_in; ///< intermediate relative tolerance
  /** @} */

  /**
   * @name Primal infeasibility variables
   * @{
   */
  c_float *delta_y;   ///< difference of consecutive dual iterates
  c_float *Atdelta_y; ///< A' * delta_y

  /** @} */

  /**
   * @name Dual infeasibility variables
   * @{
   */
  c_float *delta_x;  ///< difference of consecutive primal iterates
  c_float *Qdelta_x; ///< Q * delta_x
  c_float *Adelta_x; ///< A * delta_x

  /** @} */

  /**
   * @name Temporary vectors used in scaling
   * @{
   */

  c_float *D_temp;   ///< temporary primal variable scaling vectors
  c_float *E_temp;   ///< temporary constraints scaling vectors


  /** @} */
  QPALMSolver   *solver;   ///< linsys variables
  QPALMSettings *settings; ///< problem settings
  QPALMScaling  *scaling;  ///< scaling vectors
  QPALMSolution *solution; ///< problem solution
  QPALMInfo     *info;     ///< solver information

  # ifdef QPALM_TIMING
  QPALMTimer *timer;       ///< timer object
  # endif // ifdef QPALM_TIMING

} QPALMWorkspace;


# ifdef __cplusplus
}
# endif 

#endif // ifndef QPALM_TYPES_H
