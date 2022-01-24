/**
 * @file termination.h
 * @author Ben Hermans
 * @brief Routines to check the termination and infeasibility criteria.
 * @details The routines in this file compute the primal and dual residuals, 
 * the primal and dual tolerances, check whether the problem is solved 
 * completely, unscale and store the solution if that is the case, check
 *  whether the intermediate problem is solved and whether one of the 
 * infeasibility criteria hold. In other words, all routines related to 
 * the termination of the optimization algorithm are grouped in this file.
 */
#ifndef TERMINATION_H
# define TERMINATION_H

# ifdef __cplusplus
extern "C" {
# endif 

#include "types.h"

/**
 * Calls the routines that compute the norm of the residuals and the tolerances.
 * 
 * @param work Workspace
 */
void calculate_residual_norms_and_tolerances(QPALMWorkspace *work);

/**
 * Calculates the infinity norm of the primal residual and stores it in work->info->pri_res_norm.
 * 
 * The primal residual is given by @f$Ax-z@f$. In the scaled case, the
 * residual used for checking is unscaled and is @f$E^{-1}(Ax-z)@f$ instead.
 * 
 * @param work Workspace
 */
void calculate_primal_residual(QPALMWorkspace *work);

/**
 * Calculates the infinity norm of the dual residual and the residual of the subproblem 
 * and stores them in work->info->dua_res_norm and work->info->dua2_res_norm respectively.
 * 
 * The dual residual is given by @f$\nabla \varphi@f$. In the scaled case, the
 * residual used for checking is unscaled and is @f$D^{-1}\nabla \varphi@f$ instead.
 * If the work->settings->proximal is true, then the dual residual is 
 * @f$\nabla \varphi - \frac{1}{\gamma}(x-x_0)@f$. If work->settings->proximal is true
 * and the problem is scaled, then the dual residual is 
 * @f$D^{-1}(\nabla \varphi - \frac{1}{\gamma}(x-x_0))@f$.
 * The residual of the subproblem is @f$\nabla \varphi@f$ or @f$D^{-1}\nabla \varphi@f$
 * in the scaled case.
 * 
 * @param work Workspace
 */
void calculate_dual_residuals(QPALMWorkspace *work);

/**
 * Calculates the primal tolerance and stores it in work->eps_pri.
 * 
 * For the primal tolerance an absolute and relative contribution are added. 
 * It is given by 
 * @f$\epsilon_\textrm{abs} + \epsilon_\textrm{rel} \textrm{max}(\|Ax\|_\infty, \|z\|_\infty)@f$.
 * In the scaled case, the relative contributions are unscaled and the primal
 * tolerance is given by
 * @f$\epsilon_\textrm{abs} + \epsilon_\textrm{rel} \textrm{max}(\|E^{-1}Ax\|_\infty, \|E^{-1}z\|_\infty)@f$.
 * 
 * @param work Workspace
 */
void calculate_primal_tolerance(QPALMWorkspace *work);

/**
 * Calculates the dual tolerance for the problem and current subproblem and stores them 
 * in work->eps_dua and worl->eps_dua_in respectively.
 * 
 * For the dual tolerance an absolute and relative contribution are added. 
 * It is given by 
 * @f$\epsilon_\textrm{abs} + \epsilon_\textrm{rel} \textrm{max}(\|Qx\|_\infty, \|q\|_\infty, \|A^T y\|_\infty)@f$.
 * In the scaled case, the relative contributions are unscaled and the primal
 * tolerance is given by
 * @f$\epsilon_\textrm{abs} + \epsilon_\textrm{rel}c^{-1}\textrm{max}(\|D^{-1}Qx\|_\infty, \|D^{-1}q\|_\infty, \|D^{-1}A^T y\|_\infty)@f$.
 * The tolerance for the subproblem is given by the same formulas, with @f$\epsilon_\textrm{abs,in}@f$
 * and @f$\epsilon_\textrm{rel,in}@f$ instead of @f$\epsilon_\textrm{abs}@f$ and @f$\epsilon_\textrm{rel}@f$.
 * 
 * @param work Workspace
 */
void calculate_dual_tolerances(QPALMWorkspace *work);

/**
 * Check whether the primal and dual residual norms are smaller than the primal and dual tolerances.
 * 
 * @param work Workspace
 * @return Exitflag indicating whether the problem is solved.
 */
c_int is_solved(QPALMWorkspace *work);

/**
 * Check whether the problem is primal infeasible.
 * 
 * The infeasibility criterion used here was introduced in \cite osqp-infeasibility. 
 * The problem is primal infeasible if for @f$\delta y = \Sigma\bigl(Ax-z)\bigr) \neq 0@f$, with 
 * @f$\Sigma = \textrm{diag}(\sigma)@f$ the following condtions hold:
 * @f{align*}{
 *	\|A^T \delta y\|_\infty & \leq \varepsilon_\textrm{prim,inf} \|\delta y\|_\infty,
 *	\\
 *	b_\textrm{max}^T [\delta y]_+ + b_\textrm{min}^T [\delta y]_- & \leq -\varepsilon_\textrm{prim,inf} \|\delta y\|_\infty.
 *  @f}
 * In case the problems is scaled the conditions turn into the following:
 * @f{align*}{
 *	\|D^{-1}A^T \delta y\|_\infty & \leq \varepsilon_\textrm{prim,inf} \|E\delta y\|_\infty,
 *	\\
 *	b_\textrm{max}^T [\delta y]_+ + b_\textrm{min}^T [\delta y]_- & \leq -\varepsilon_\textrm{prim,inf} \|E\delta y\|_\infty.
 *  @f}
 * 
 * If the above conditions hold, @f$\delta y@f$, or in the scaled case @f$c^{-1} E\delta y@f$, is the certificate of primal infeasibility.
 * 
 * @param work Workspace
 * @return Exitflag indicating whether the problem is primal infeasible.
 */
c_int is_primal_infeasible(QPALMWorkspace *work);


/**
 * Check whether the problem is dual infeasible.
 * 
 * The infeasibility criterion used here was introduced in \cite osqp-infeasibility. 
 * The problem is dual infeasible if for @f$\delta x = x-x_\textrm{prev} \neq 0@f$ the following condtions hold:
 * @f{align*}{
 * \|Q\delta x\|_\infty &\leq \varepsilon_\textrm{dua, inf} \|\delta x\|_\infty, \\
 * q^T \delta x & \leq - \varepsilon_\textrm{dua, inf} \|\delta x\|_\infty, 
 *  @f}
 * 
 * @f{align*}{
	(A\delta x)_i = 
	\begin{cases}
		\geq \varepsilon_{\rm dua,inf} \|\delta x\|_\infty & \textrm{if } b_{\textrm{max},i} = +\infty
	\\
		\leq -\varepsilon_{\rm dua,inf} \|\delta x\|_\infty & \textrm{if } b_{\textrm{min},i} = -\infty
	\\
		\in [-\varepsilon_{\rm dua,inf},\varepsilon_{\rm dua,inf}] \|\delta x\|_\infty & \textrm{if } b_{\textrm{max},i}, b_{\textrm{min},i} \in {\rm I\!R}.
	\end{cases}
 * @f}
 * In case the problems is scaled the conditions turn into the following:
 * @f{align*}{
 * \|D^{-1}Q\delta x\|_\infty &\leq c \cdot \varepsilon_\textrm{dua, inf} \|D\delta x\|_\infty, \\
 * q^T \delta x & \leq - c \cdot \varepsilon_\textrm{dua, inf} \|D\delta x\|_\infty, 
 *  @f}
 * 
 * @f{align*}{
	(E^{-1}A\delta x)_i = 
	\begin{cases}
		\geq \varepsilon_{\rm dua,inf} \|D\delta x\|_\infty & \textrm{if } b_{\textrm{max},i} = +\infty
	\\
		\leq -\varepsilon_{\rm dua,inf} \|D\delta x\|_\infty & \textrm{if } b_{\textrm{min},i} = -\infty
	\\
		\in [-\varepsilon_{\rm dua,inf},\varepsilon_{\rm dua,inf}] \|D\delta x\|_\infty & \textrm{if } b_{\textrm{max},i}, b_{\textrm{min},i} \in {\rm I\!R}.
	\end{cases}
 * @f}
 * 
 * If the above conditions hold, @f$\delta x@f$, or in the scaled case @f$D\delta x@f$, is the certificate of dual infeasibility.
 * 
 * @param work Workspace
 * @return Exitflag indicating whether the problem is dual infeasible.
 */
c_int is_dual_infeasible(QPALMWorkspace *work);

/**
 * Check whether the subproblem is solved.
 * 
 * This amounts to checking whether the residual of the subproblem smaller than or 
 * equal is to the tolerance of the subproblem, as mentioned in calculate_dual_residuals
 * and calculate_dual_tolerances.
 * 
 * @param work Workspace
 * @return Exitflag indicating whether the subproblem is solved.
 * 
 */
c_int check_subproblem_termination(QPALMWorkspace *work);


/**
 * 
 * Helper function to store the (unscaled) solution in the solution struct.
 * 
 * The primal/dual solution @f$x,y@f$ is copied to work->solution->x, work->solution->y.
 * In case the problem is scaled, the soltution is first unscaled, and the solution vectors
 * @f$Dx, c^{-1}Ey@f$ are stored in work->solution->x, work->solution->y. 
 * 
 * @param work Workspace
 */
void store_solution(QPALMWorkspace *work);



# ifdef __cplusplus
}
# endif 

#endif // ifndef TERMINATION_H