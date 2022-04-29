/**
 * @file newton.c
 * @author Ben Hermans
 * @brief Functions to calculate the semismooth Newton direction.
 * @details The functions in this file concern the calculation of the semismooth Newton direction. 
 * Factorizing, updating the factorization and solving the linear system are performed by functions in 
 * solver_interface.c. 
 */
# ifdef __cplusplus
extern "C" {
# endif // ifdef __cplusplus

#include "newton.h"
#include "lin_alg.h"
#include <stdio.h>

void newton_set_direction(QPALMWorkspace *work, solver_common *c) {

    set_active_constraints(work);
    set_entering_leaving_constraints(work);
    
    if (work->solver->factorization_method == FACTORIZE_KKT)
    {
        #ifdef QPALM_USE_LADEL 
        // NB FACTORIZE_KKT only defined with LADEL for now
        // TODO (optionally) extract the ladel functions and bury them, use cholmod also for kkt
        ladel_diag d;
        d.diag_elem = 1.0/work->gamma;
        if (work->settings->proximal) d.diag_size = work->data->n;
        else d.diag_size = 0;

        if (work->solver->first_factorization)
        {
            qpalm_form_kkt(work);
            work->solver->LD = ladel_factor_free(work->solver->LD);
            ladel_factorize_advanced_with_diag(work->solver->kkt, d, work->solver->sym, work->settings->ordering, &work->solver->LD, work->solver->kkt_full, c);
            work->solver->first_factorization = FALSE;
        } 
        else if (work->solver->reset_newton || 
                (work->solver->nb_enter + work->solver->nb_leave) > 
                    c_min(work->settings->max_rank_update_fraction*(work->data->n+work->data->m), work->settings->max_rank_update))
        {
            qpalm_reform_kkt(work);
            ladel_factorize_with_prior_basis_with_diag(work->solver->kkt, d, work->solver->sym, work->solver->LD, c);
        }
        else 
        {
            if(work->solver->nb_enter) 
                kkt_update_entering_constraints(work, c);

            if (work->solver->nb_leave)
                kkt_update_leaving_constraints(work, c);
        }

        kkt_solve(work, c);

        // /* Iterative refinement: compute r = b - Ax = -dphi - kkt*sol and add sol += A\r */
        mat_vec(work->solver->kkt, work->solver->sol_kkt, work->solver->rhs_kkt, c);
        if(work->settings->proximal) vec_mult_add_scaled(work->solver->rhs_kkt, work->solver->sol_kkt, 1, 1.0/work->gamma, work->data->n);
        vec_self_mult_scalar(work->solver->rhs_kkt, -1, work->data->m + work->data->n);
        c_float ref_norm = c_max(vec_norm_inf(work->solver->rhs_kkt, work->data->n + work->data->m), vec_norm_inf(work->dphi, work->data->n));
        vec_mult_add_scaled(work->solver->rhs_kkt, work->dphi, 1, -1, work->data->n);

        c_float first_res = vec_norm_inf(work->solver->rhs_kkt, work->data->n + work->data->m);
        c_float res = first_res;
        c_int k = 0;

        // if(res > RELATIVE_REFINEMENT_TOLERANCE*ref_norm) ladel_print("ref_norm: %e\n", ref_norm);

        while(k < MAX_REFINEMENT_ITERATIONS && res > c_max(RELATIVE_REFINEMENT_TOLERANCE*ref_norm, ABSOLUTE_REFINEMENT_TOLERANCE))
        {
            k++;
            // ladel_print("Refinement because relative res = %e (ref_norm = %e)\n", vec_norm_inf(work->solver->rhs_kkt, work->data->n + work->data->m)/ref_norm, ref_norm);
            prea_vec_copy(work->solver->sol_kkt, work->temp_n, work->data->n);
            prea_vec_copy(work->solver->sol_kkt + work->data->n, work->temp_m, work->data->m);
            ladel_dense_solve(work->solver->LD, work->solver->rhs_kkt, work->solver->sol_kkt, c);
            vec_add_scaled(work->solver->sol_kkt, work->d, work->d, 1, work->data->n);

            vec_mult_add_scaled(work->solver->sol_kkt, work->temp_n, 1, 1, work->data->n);
            vec_mult_add_scaled(work->solver->sol_kkt + work->data->n, work->temp_m, 1, 1, work->data->m);
            mat_vec(work->solver->kkt, work->solver->sol_kkt, work->solver->rhs_kkt, c);
            if(work->settings->proximal) vec_mult_add_scaled(work->solver->rhs_kkt, work->solver->sol_kkt, 1, 1.0/work->gamma, work->data->n);
            vec_self_mult_scalar(work->solver->rhs_kkt, -1, work->data->m + work->data->n);
            vec_mult_add_scaled(work->solver->rhs_kkt, work->dphi, 1, -1, work->data->n);
            res = vec_norm_inf(work->solver->rhs_kkt, work->data->n + work->data->m);
            // if(res > RELATIVE_REFINEMENT_TOLERANCE*ref_norm)
            //     ladel_print("K = %d, Res = %e (first res = %e)\n", k, res/ref_norm, first_res/ref_norm);
            // else
            //     ladel_print("FINAL K = %d, Res = %e (first res = %e)\n", k, res/ref_norm, first_res/ref_norm);
        }

        // if (k==MAX_REFINEMENT_ITERATIONS) ladel_print("ITREF FAILED, final res = %e (first res = %e)\n", res/ref_norm, first_res/ref_norm);


        #endif
    } else if (work->solver->factorization_method == FACTORIZE_SCHUR)
    {
        if ((work->solver->reset_newton && work->solver->nb_active_constraints) || 
            (work->solver->nb_enter + work->solver->nb_leave) > 
                c_min(work->settings->max_rank_update_fraction*(work->data->n+work->data->m), work->settings->max_rank_update)) {
            ldlcholQAtsigmaA(work, c);   
        } else if (work->solver->nb_active_constraints) {
            if(work->solver->nb_enter) {
                ldlupdate_entering_constraints(work, c);
            }
            if(work->solver->nb_leave) {
                ldldowndate_leaving_constraints(work, c); 
            }
        } else {
            ldlchol(work->data->Q, work, c);
        }
        ldlsolveLD_neg_dphi(work, c);
    } 
    
    //Store old active set
    prea_int_vec_copy(work->solver->active_constraints, work->solver->active_constraints_old, work->data->m);

    work->solver->reset_newton = FALSE;

}

void set_active_constraints(QPALMWorkspace *work) {
    work->solver->nb_active_constraints = 0;
    for (size_t i = 0; i < work->data->m; i++) {
        if ((work->Axys[i] <= work->data->bmin[i]) || ((work->Axys[i] >= work->data->bmax[i]))){
            work->solver->active_constraints[i] = TRUE;
            work->solver->nb_active_constraints++;
        } else {
            work->solver->active_constraints[i] = FALSE;
        }         
    }
}

void set_entering_leaving_constraints(QPALMWorkspace *work) {
    int nb_enter = 0;
    int nb_leave = 0;
    for (size_t i = 0; i < work->data->m; i++) {
        if (work->solver->active_constraints[i] && !work->solver->active_constraints_old[i]) {
            work->solver->enter[nb_enter] = (c_int)i;
            nb_enter++;
        }
        if (!work->solver->active_constraints[i] && work->solver->active_constraints_old[i]) {
            work->solver->leave[nb_leave] = (c_int)i;
            nb_leave++;
        }
    }
    work->solver->nb_enter = nb_enter;
    work->solver->nb_leave = nb_leave;
}



# ifdef __cplusplus
}
# endif // ifdef __cplusplus