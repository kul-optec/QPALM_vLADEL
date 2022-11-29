/**
 * @file util.c
 * @author Ben Hermans
 * @brief Utility functions.
 * @details This file contains some utility functions, to copy the settings, 
 * to update the solver status, to print information and to time the algorithm.
 */

#include "util.h"
#include "lin_alg.h"
#include "global_opts.h"
#include "string.h"
#include "types.h"
#include <stdio.h>
/**********************
* Utility Functions  *
**********************/

void c_strcpy(char dest[], const char source[]) {
    size_t i;
    for(i = 0; (dest[i] = source[i]) != '\0'; i++);
}


QPALMSettings* copy_settings(const QPALMSettings *settings) {
    QPALMSettings *new = qpalm_malloc(sizeof(QPALMSettings));

    // Copy settings
    new->max_iter                 = settings->max_iter; 
    new->inner_max_iter           = settings->inner_max_iter;    
    new->eps_abs                  = settings->eps_abs;       
    new->eps_rel                  = settings->eps_rel;       
    new->eps_abs_in               = settings->eps_abs_in;    
    new->eps_rel_in               = settings->eps_rel_in;    
    new->rho                      = settings->rho;           
    new->eps_prim_inf             = settings->eps_prim_inf;  
    new->eps_dual_inf             = settings->eps_dual_inf; 
    new->theta                    = settings->theta;         
    new->delta                    = settings->delta;
    new->sigma_max                = settings->sigma_max;
    new->sigma_init               = settings->sigma_init;
    new->proximal                 = settings->proximal;       
    new->gamma_init               = settings->gamma_init;         
    new->gamma_upd                = settings->gamma_upd;     
    new->gamma_max                = settings->gamma_max;     
    new->scaling                  = settings->scaling;    
    new->nonconvex                = settings->nonconvex;  
    new->verbose                  = settings->verbose;
    new->print_iter               = settings->print_iter; 
    new->warm_start               = settings->warm_start;
    new->reset_newton_iter        = settings->reset_newton_iter;
    new->enable_dual_termination  = settings->enable_dual_termination;
    new->dual_objective_limit     = settings->dual_objective_limit;
    new->time_limit               = settings->time_limit;
    new->ordering                 = settings->ordering;
    new->factorization_method     = settings->factorization_method;
    new->max_rank_update          = settings->max_rank_update;
    new->max_rank_update_fraction = settings->max_rank_update_fraction;
    return new;
}

void update_status(QPALMInfo *info, c_int status_val) {
    // Update status value
    info->status_val = status_val;

    // Update status string depending on status val
    switch (status_val)
    {
    case QPALM_SOLVED:
      c_strcpy(info->status, "solved");
      break;
    case QPALM_DUAL_TERMINATED:
      c_strcpy(info->status, "dual terminated");
      break;
    case QPALM_PRIMAL_INFEASIBLE:
      c_strcpy(info->status, "primal infeasible");
      break;
    case QPALM_DUAL_INFEASIBLE:
      c_strcpy(info->status, "dual infeasible");
      break;
    case QPALM_TIME_LIMIT_REACHED:
      c_strcpy(info->status, "time limit exceeded");
      break;
    case QPALM_MAX_ITER_REACHED:
      c_strcpy(info->status, "maximum iterations reached");
      break;
    case QPALM_UNSOLVED:
      c_strcpy(info->status, "unsolved");
      break;
    case QPALM_ERROR:
      c_strcpy(info->status, "error");
      break;
    default:
      c_strcpy(info->status, "unrecognised status value");
      #ifdef QPALM_PRINTING
        qpalm_eprint("Unrecognised status value %" LADEL_PRIi, status_val);
      #endif
      break;
    }
}

/**********************
* Print Functions  *
**********************/

#ifdef QPALM_PRINTING

void print_header(void) {
    qpalm_print("\n                  QPALM Version " QPALM_VERSION_STR "                   \n\n");
    qpalm_print("Iter |   P. res   |   D. res   |  Stepsize  |  Objective  \n");
    qpalm_print("==========================================================\n");
}

void print_iteration(c_int iter, QPALMWorkspace *work) {
    qpalm_print("%4" LADEL_PRIi " | %.4e | %.4e | %.4e | %.4e \n", iter,
                                                    work->info->pri_res_norm,
                                                    work->info->dua_res_norm,
                                                    work->tau,
                                                    work->info->objective);
}

void print_final_message(QPALMWorkspace *work) {
    qpalm_print("\n\n=============================================================\n");
    size_t characters_box;
    char buf[80];
    switch (work->info->status_val) {
      case QPALM_SOLVED:
      snprintf(buf, 80, "| QPALM finished successfully.                              |\n");
      characters_box = strlen(buf);
      qpalm_print("%s", buf);
        // characters_box =  qpalm_print("| QPALM finished successfully.                              |\n");
                          qpalm_print("| primal residual: %5.4e, primal tolerance: %5.4e |\n", work->info->pri_res_norm, work->eps_pri);
                          qpalm_print("| dual residual  : %5.4e, dual tolerance  : %5.4e |\n", work->info->dua_res_norm, work->eps_dua);
                          qpalm_print("| objective value: %+-5.4e                              |\n", work->info->objective);
        break;
      case QPALM_DUAL_TERMINATED:
      snprintf(buf, 80,"| QPALM has terminated because the dual objective at the    |\n");
      characters_box = strlen(buf);
      qpalm_print("%s", buf);
        // characters_box =  qpalm_print("| QPALM has terminated because the dual objective at the    |\n");
                          qpalm_print("| current iterate is higher than the value specified in     |\n");
                          qpalm_print("| dual_objective_limit.                                     |\n");
                          qpalm_print("| dual objective : %+-4.3e, specified limit : %+-4.3e |\n", work->info->dual_objective, work->settings->dual_objective_limit);
        break;
      case QPALM_PRIMAL_INFEASIBLE:
      snprintf(buf, 80,"| QPALM detected a primal infeasible problem. You can check |\n");
      characters_box = strlen(buf);
      qpalm_print("%s", buf);
        // characters_box =  qpalm_print("| QPALM detected a primal infeasible problem. You can check |\n");
                          qpalm_print("| the certificate of this infeasiblity. If you think the    |\n");
                          qpalm_print("| problem might not be infeasible, try lowering the         |\n");
                          qpalm_print("| infeasiblity tolerance eps_prim_inf.                      |\n");
        break;
      case QPALM_DUAL_INFEASIBLE:
      snprintf(buf, 80,"| QPALM detected a dual infeasible problem. You can check   |\n");
      characters_box = strlen(buf);
      qpalm_print("%s", buf);
        // characters_box =  qpalm_print("| QPALM detected a dual infeasible problem. You can check   |\n");
                          qpalm_print("| the certificate of this infeasiblity. If you think the    |\n");
                          qpalm_print("| problem might not be dual infeasible, try lowering the    |\n");
                          qpalm_print("| infeasiblity tolerance eps_dual_inf.                      |\n");
        break;
      case QPALM_MAX_ITER_REACHED:
      snprintf(buf, 80,"| QPALM hit the maximum number of iterations.               |\n");
      characters_box = strlen(buf);
      qpalm_print("%s", buf);
        // characters_box =  qpalm_print("| QPALM hit the maximum number of iterations.               |\n");
                          qpalm_print("| primal residual: %5.4e, primal tolerance: %5.4e |\n", work->info->pri_res_norm, work->eps_pri);
                          qpalm_print("| dual residual  : %5.4e, dual tolerance  : %5.4e |\n", work->info->dua_res_norm, work->eps_dua);
                          qpalm_print("| objective value: %+-5.4e                              |\n", work->info->objective);
        break;
      case QPALM_TIME_LIMIT_REACHED:
      snprintf(buf, 80,"| QPALM has exceeded the specified time limit.              |\n");
      characters_box = strlen(buf);
      qpalm_print("%s", buf);
        // characters_box =  qpalm_print("| QPALM has exceeded the specified time limit.              |\n");
                          qpalm_print("| primal residual: %5.4e, primal tolerance: %5.4e |\n", work->info->pri_res_norm, work->eps_pri);
                          qpalm_print("| dual residual  : %5.4e, dual tolerance  : %5.4e |\n", work->info->dua_res_norm, work->eps_dua);
                          qpalm_print("| objective value: %+-5.4e                              |\n", work->info->objective);
        break;
      default:
        c_strcpy(work->info->status, "unrecognised status value");
        qpalm_eprint("Unrecognised final status value %" LADEL_PRIi, work->info->status_val);
        return;
    }
    #ifdef QPALM_TIMING
    size_t characters_runtime;
    if (work->info->run_time > 1.0) {
      snprintf(buf, 80,"| runtime:         %4.2f seconds", work->info->run_time);
      characters_runtime = strlen(buf);
      qpalm_print("%s", buf);
      // characters_runtime = qpalm_print("| runtime:         %4.2f seconds", work->info->run_time);
    } else {
      snprintf(buf, 80,"| runtime:         %4.2f milliseconds", work->info->run_time*1000);
      characters_runtime = strlen(buf);
      qpalm_print("%s", buf);
      // characters_runtime = qpalm_print("| runtime:         %4.2f milliseconds", work->info->run_time*1000);
    }
    for (; characters_runtime < characters_box-2; characters_runtime++) {
      qpalm_print(" ");
    }
    qpalm_print("|\n");
    #endif
    
    qpalm_print("=============================================================\n");
    qpalm_print("\n\n");
}

#endif //Printing

/*******************
* Timer Functions *
*******************/

#ifdef QPALM_TIMING

// Windows
# ifdef _WIN32

void qpalm_tic(QPALMTimer *t)
{
  QueryPerformanceFrequency(&t->freq);
  QueryPerformanceCounter(&t->tic);
}

c_float qpalm_toc(QPALMTimer *t)
{
  QueryPerformanceCounter(&t->toc);
  return (t->toc.QuadPart - t->tic.QuadPart) / (c_float)t->freq.QuadPart;
}

// Mac
# elif defined __APPLE__

void qpalm_tic(QPALMTimer *t)
{
  /* read current clock cycles */
  t->tic = mach_absolute_time();
}

c_float qpalm_toc(QPALMTimer *t)
{
  uint64_t duration; /* elapsed time in clock cycles*/

  t->toc   = mach_absolute_time();
  duration = t->toc - t->tic;

  /*conversion from clock cycles to nanoseconds*/
  mach_timebase_info(&(t->tinfo));
  duration *= t->tinfo.numer;
  duration /= t->tinfo.denom;

  return (c_float)duration / 1e9;
}

// Mac
# elif defined __MACH__

void qpalm_tic(QPALMTimer *t)
{
  /* read current clock cycles */
  t->tic = mach_absolute_time();
}

c_float qpalm_toc(QPALMTimer *t)
{
  uint64_t duration; /* elapsed time in clock cycles*/

  t->toc   = mach_absolute_time();
  duration = t->toc - t->tic;

  /*conversion from clock cycles to nanoseconds*/
  mach_timebase_info(&(t->tinfo));
  duration *= t->tinfo.numer;
  duration /= t->tinfo.denom;

  return (c_float)duration / 1e9;
}

// Linux
# elif defined __linux__  /* ifdef _WIN32 */
/* read current time */

void qpalm_tic(QPALMTimer *t)
{
  clock_gettime(CLOCK_MONOTONIC, &t->tic);
}

/* return time passed since last call to tic on this timer */
c_float qpalm_toc(QPALMTimer *t)
{
  struct timespec temp;

  clock_gettime(CLOCK_MONOTONIC, &t->toc);

  if ((t->toc.tv_nsec - t->tic.tv_nsec) < 0) {
    temp.tv_sec  = t->toc.tv_sec - t->tic.tv_sec - 1;
    temp.tv_nsec = 1000000000 + t->toc.tv_nsec - t->tic.tv_nsec;
  } else {
    temp.tv_sec  = t->toc.tv_sec - t->tic.tv_sec;
    temp.tv_nsec = t->toc.tv_nsec - t->tic.tv_nsec;
  }
  return (c_float)temp.tv_sec + (c_float)temp.tv_nsec / 1e9;
}

# endif /* ifdef IS_WINDOWS */

#endif // If Profiling end
