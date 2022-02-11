/**
 * @file newton.h
 * @author Ben Hermans
 * @brief Functions to calculate the semismooth Newton direction.
 * @details The functions in this file concern the calculation of the semismooth Newton direction. 
 * Factorizing, updating the factorization and solving the linear system are performed by functions in 
 * solver_interface.c. 
 */

#ifndef NEWTON_H
#define NEWTON_H

# ifdef __cplusplus
extern "C" {
# endif 

#include "types.h"

/**
 * Sets work->d to the direction calculated by the semismooth Newton method
 * 
 * @param work  Workspace
 * @param c     Linear systems solver environment 
 */
void newton_set_direction(  QPALMWorkspace  *work, 
                            solver_common   *c);

/**
 * Computes the set of active constraints and stores it in work->solver->active_constraints.
 * 
 * @param work Workspace
 */
void set_active_constraints(QPALMWorkspace *work);

/**
 * Determines the entering and leaving constraints and stores them in work->solver->enter and work->solver->leave respectively.
 * 
 * Entering constraints are constraints that are in the new but not in the previous set of active constraints.
 * Leaving constraints are constraints that are in the previous but not in the new set of active constraints.
 * 
 * @param work Workspace
 */
void set_entering_leaving_constraints(QPALMWorkspace *work);

# ifdef __cplusplus
}
# endif 

#endif //ifndef NEWTON_H