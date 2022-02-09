/**
 * @file qpalm.h
 * @author Ben Hermans
 * @brief QPALM main solver API.
 * @details This file contains the main functions that can be called by the user.
 * The user can load the default settings, setup the workspace with data and settings,
 * warm_start the primal and dual variables, run the solver, update the settings, bounds 
 * and linear part of the cost, and finally cleanup the workspace afterwards.
 */

#ifndef QPALM_H
#define QPALM_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "constants.h"
#include "global_opts.h"
#include "iteration.h"
#include "lin_alg.h"
#include "linesearch.h"
#include "newton.h"
#include "nonconvex.h"
#include "scaling.h"
#include "solver_interface.h"
#include "termination.h"
#include "types.h"
#include "util.h"
#include "validate.h"


/********************
* Main Solver API  *
********************/

/**
 * @defgroup solver-grp Main solver API
 * @brief   The main C API of the QPALM solver.
 * @{
 */

/**
 * Set default settings from constants.h file.
 * Assumes settings are already allocated in memory.
 * @param settings Settings structure
 */
QPALM_EXPORT void qpalm_set_default_settings(QPALMSettings *settings);


/**
 * Initialize QPALM solver allocating memory.
 *
 * All the inputs must be already allocated in memory before calling.
 *
 * It performs:
 * - data and settings validation
 * - problem data scaling
 *
 * @param  data         Problem data
 * @param  settings     Solver settings
 * @return              Solver environment
 */
QPALM_EXPORT QPALMWorkspace* qpalm_setup(const QPALMData     *data,
                            const QPALMSettings *settings);


/**
 * Warm start workspace variables x, x_0, x_prev, Ax, Qx, y and sigma
 * 
 * If x_warm_start or y_warm_start is given as NULL, then the related variables
 * will be initialized to 0. This function also initializes the penalty parameters
 * sigma and the matrix Asqrtsigma.
 * 
 * @param work Workspace
 * @param x_warm_start Warm start for the primal variables
 * @param y_warm_start Warm start for the dual variables
 */
QPALM_EXPORT void qpalm_warm_start(QPALMWorkspace *work, 
                      const c_float  *x_warm_start, 
                      const c_float  *y_warm_start);

/**
 * Solve the quadratic program.
 *
 * The final solver information is stored in the \a work->info structure.
 *
 * The solution is stored in the \a work->solution structure.
 *
 * If the problem is primal infeasible, the certificate is stored
 * in \a work->delta_y.
 *
 * If the problem is dual infeasible, the certificate is stored in \a
 * work->delta_x.
 *
 * @param  work Workspace
 */
QPALM_EXPORT void qpalm_solve(QPALMWorkspace *work);


/**
 * Update the settings to the new settings.
 * 
 * @warning Decreasing settings->scaling is not allowed. Increasing it is possible.
 * 
 * @param work Workspace
 * @param settings New settings
 */
QPALM_EXPORT void qpalm_update_settings(QPALMWorkspace      *work, 
                           const QPALMSettings *settings);

/**
 * Update the lower and upper bounds.
 * 
 * Use NULL to indicate that one of the bounds does not change.
 * 
 * @param work Workspace
 * @param bmin New lower bounds
 * @param bmax New upper bounds
 */
QPALM_EXPORT void qpalm_update_bounds(QPALMWorkspace *work,
                         const c_float  *bmin, 
                         const c_float  *bmax);

/**
 * Update the linear part of the cost.
 * 
 * This causes an update of the cost scaling factor as well.
 * 
 * @param work Workspace
 * @param q Linear part of the objective
 */
QPALM_EXPORT void qpalm_update_q(QPALMWorkspace  *work, 
                    const c_float   *q);

/**
 * Update the matrix entries of Q and A.
 * 
 * This function does not allow a change in the patterns of Q and A. For this, the
 * user will need to recall qpalm_setup.
 * 
 * @param work Workspace
 * @param Qx Elements of Q (upper diagonal part)
 * @param Ax Elements of A
 */
QPALM_EXPORT void qpalm_update_Q_A(QPALMWorkspace *work, 
                      const c_float  *Qx, 
                      const c_float  *Ax);



/**
 * Cleanup the workspace by deallocating memory.
 *
 * This function should be the called after the user is done using QPALM.
 * @param  work Workspace
 */
QPALM_EXPORT void qpalm_cleanup(QPALMWorkspace *work);

/**
 * @}
 */

# ifdef __cplusplus
}
# endif 

#endif /* QPALM_H */