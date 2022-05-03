/**
 * @file nonconvex.c
 * @author Ben Hermans
 * @brief Routines to deal with nonconvex QPs.
 * @details The functions in this file serve to set up QPALM for a nonconvex QP. The main routine in 
 * this file computes the minimum eigenvalue of a square matrix, based on lobpcg \cite knyazev2001toward. Furthermore, 
 * some setting updates are performed. In addition, the spectrum of a matrix can be upper bounded using 
 * Gershgorin's circle theorem, which is used in the gamma_boost routine in iteration.c.
 */

#include "nonconvex.h"
#include "types.h"
#include "constants.h"
#include "global_opts.h"
#include "lin_alg.h"
#include "util.h"

#ifdef QPALM_NONCONVEX
#include <math.h>

/*TODO: make this a setting */
#define LOBPCG_TOL 1e-5 /**< Tolerance on the infinity norm of the residual in lobpcg. */ 

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define RREF_TOL 1e-8

static c_float min_root_third_order(c_float a, c_float b, c_float c, c_float d)
{
    c_float r[3] = {0};
    c_float di, di_sqrt;
    if (a == 0)
    {
        // Not a cubic polynomial, should not happen 
        # ifdef QPALM_PRINTING
        qpalm_eprint("Not a cubic polynomial.");
        # endif /* ifdef QPALM_PRINTING */
    }
    else if (d == 0)
    {
        di = b*b - 4*a*c;
        if (di < 0)
        {
            # ifdef QPALM_PRINTING
            qpalm_eprint("Imaginary roots. This should not happen.");
            # endif /* ifdef QPALM_PRINTING */
        }
        di_sqrt = c_sqrt(di);
        r[0] = (-b-di_sqrt)/(2*a);
        r[1] = (-b+di_sqrt)/(2*a); 
    }
    else
    {
        c_float temp, q, p, re, an, r13;
        temp = 1/a;
        b = b*temp;
        c = c*temp;
        d = d*temp;
        q = (3.0*c - (b*b))/9.0;
        p = (-(27.0*d) + b*(9.0*c - 2.0*(b*b)))/54.0;
        di = q*q*q + p*p;
        re = b/3.0;
        if (di > 0)
        {
            # ifdef QPALM_PRINTING
            qpalm_eprint("Imaginary roots. This should not happen.");
            # endif /* ifdef QPALM_PRINTING */
        }   
        else 
        {
            q = -q;
            an = q*q*q;
            an = c_acos(p/c_sqrt(an));
            r13 = 2.0*c_sqrt(q);
            r[0] = -re + r13*c_cos(an/3.0);
            r[1] = -re + r13*c_cos((an + 2.0*M_PI)/3.0);
            r[2] = -re + r13*c_cos((an + 4.0*M_PI)/3.0);
        }
    }
    
    if (r[0] <= r[1] && r[0] <= r[2]) return r[0];
    else return c_min(r[1], r[2]);
}


static int custom_rref(c_float D[3][3])
{
    c_float p, temp[3], a[3];

    // First column
    a[0] = c_absval(D[0][0]); a[1] = c_absval(D[0][1]); a[2] = c_absval(D[0][2]); 
    if (a[0] < a[1] || a[0] < a[2])
    {
        if (a[1] > a[2])
        {
            if (a[1] < RREF_TOL) return 0;
            // swap row 0 and 1
            temp[0] = D[0][0]; temp[1] = D[0][1]; temp[2] = D[0][2];
            D[0][0] = D[1][0]; D[0][1] = D[1][1]; D[0][2] = D[1][2];
            D[1][0] = temp[0]; D[1][1] = temp[1]; D[1][2] = temp[2];  
        }
        else
        {
            if (a[2] < RREF_TOL) return 0;
            // swap row 0 and 2
            temp[0] = D[0][0]; temp[1] = D[0][1]; temp[2] = D[0][2];
            D[0][0] = D[2][0]; D[0][1] = D[2][1]; D[0][2] = D[2][2];
            D[2][0] = temp[0]; D[2][1] = temp[1]; D[2][2] = temp[2];  
        }
    }
    else
    {
        if (a[0] < RREF_TOL) return 0;
    }
    
    p = 1.0/D[0][0];
    D[0][1] *= p; D[0][2] *= p; D[0][0] = 1.0;
    D[1][1] -= D[1][0]*D[0][1]; D[1][2] -= D[1][0]*D[0][2]; D[1][0] = 0;
    D[2][1] -= D[2][0]*D[0][1]; D[2][2] -= D[2][0]*D[0][2]; D[2][0] = 0;

    // Second column
    a[1] = c_absval(D[1][1]); a[2] = c_absval(D[2][1]);
    if (a[1] < a[2])
    {
        if (a[2] < RREF_TOL) return 1;
        temp[2] = D[1][2];
        D[1][1] = D[2][1]; D[1][2] = D[2][2];
        D[2][2] = temp[2]; D[2][1] = 0;
    }
    else
    {
        if (a[1] < RREF_TOL) return 1;
    }

    p = 1.0/D[1][1];
    D[1][2] *= p; D[1][1] = 1.0;
    D[0][2] -= D[0][1]*D[1][2]; D[0][1] = 0;
    D[2][2] -= D[2][1]*D[1][2]; D[2][1] = 0;

    return 2;
}

static c_float custom_eig(c_float B[3][3], c_float C[3][3], c_float x[3])
{
    c_float a, b, c, d;
    c_float xqx = B[0][0], xqw = B[0][1], xqp = B[0][2], wqw = B[1][1], wqp = B[1][2], pqp = B[2][2], xp = C[0][2], wp = C[1][2];
    a = wp*wp + xp*xp - 1;
    b = (-xqx*wp*wp + 2*xqw*wp*xp - 2*wqp*wp - wqw*xp*xp - 2*xqp*xp + pqp + wqw + xqx);
    c = (wqp*wqp - 2*xp*wqp*xqw + 2*wp*xqx*wqp + xqp*xqp - 2*wp*xqp*xqw + 2*wqw*xp*xqp + xqw*xqw - pqp*wqw - pqp*xqx - wqw*xqx);
    d = - xqx*wqp*wqp + 2*wqp*xqp*xqw - wqw*xqp*xqp - pqp*xqw*xqw + pqp*wqw*xqx;
    c_float lam = min_root_third_order(a, b, c, d);
    
    /* D = B - lam*C */
    c_float D[3][3];
    D[0][0] = B[0][0] - lam*C[0][0];
    D[0][1] = B[0][1];
    D[0][2] = B[0][2] - lam*C[0][2];
    D[1][0] = B[1][0];
    D[1][1] = B[1][1] - lam*C[1][1];
    D[1][2] = B[1][2] - lam*C[1][2];
    D[2][0] = B[2][0] - lam*C[2][0];
    D[2][1] = B[2][1] - lam*C[2][1];
    D[2][2] = B[2][2] - lam*C[2][2];
    
    int ind = custom_rref(D);

    if (ind == 0)
    {
        x[0] = 1; x[1] = 0; x[2] = 0;
    }
    else 
    if (ind == 1)
    {
        x[0] = 0; x[1] = 1; x[2] = 0;
    }
    else
    {
        c_float temp = 1/c_sqrt(1 + D[0][2]*D[0][2] - 2*D[0][2]*C[0][2] + D[1][2]*D[1][2] - 2*D[1][2]*C[1][2]);

        x[0] = -D[0][2]*temp;
        x[1] = -D[1][2]*temp;
        x[2] = temp;
    }

    return lam;
    
}

static c_float lobpcg(QPALMWorkspace *work, c_float *x, solver_common *c) {
    c_float lambda, norm_w;
    size_t i;

    size_t n = work->data->n;
    solver_sparse* A = work->data->Q;

    /*Current guess of the eigenvector */
    if (x == NULL) {
        x = work->d; 
        /* Initialize eigenvector randomly. */
        for (i = 0; i < n; i++) {
            x[i] = (c_float) rand()/(c_float) RAND_MAX;
        }
        vec_self_mult_scalar(x, 1.0/vec_norm_two(x, n), n);
    } else {
        /* NB: Assume x is already normalized */
        prea_vec_copy(x, work->d, n);
        x = work->d;
    }
    solver_dense *x_chol = work->solver->d;

    c_float *Ax = work->Qd;
    solver_dense * Ax_chol = work->solver->Qd;
    mat_vec(A, x_chol , Ax_chol, c);
    lambda = vec_prod(x, Ax, n);

    /*Current residual, Ax - lambda*x */
    c_float *w = work->neg_dphi; 
    solver_dense * w_chol = work->solver->neg_dphi;
    c_float *Aw = work->Atyh;
    solver_dense * Aw_chol = work->solver->Atyh;

    /* Conjugate gradient direction */
    c_float *p = work->temp_n; 
    c_float *Ap = work->xx0;
    c_float p_norm_inv;

    /* Compressed system B = [x, w, p]'*Q*[x, w, p] */
    c_float B[3][3]; 
    c_float C[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; /* C = [1 0 xp; 0 1 wp; xp wp 1] Takes into account that p is not orthonormal with x, w */
    c_float y[3]; /* Eigenvector corresponding to min(lambda_B) */
    c_float xAw, wAw, xAp, wAp, pAp, xp, wp;

    /* Compute residual and make it orthonormal wrt x */
    vec_add_scaled(Ax, x, w, -lambda, n);
    vec_add_scaled(w, x, w, -vec_prod(x, w, n), n);
    vec_self_mult_scalar(w, 1.0/vec_norm_two(w, n), n);
    mat_vec(A, w_chol, Aw_chol, c);
    xAw = vec_prod(Aw, x, n);
    wAw = vec_prod(Aw, w, n);

    /* In the first compressed system, there is no p yet, so it is 2 by 2 */
    c_float B_init[2][2] = {{lambda, xAw}, {xAw, wAw}};
    
    /* Solve 2x2 eigenvalue system */
    c_float b, cc, di;
    b = -(lambda + wAw);
    cc = lambda*wAw - xAw*xAw;
    di = b*b - 4*cc;
    lambda = (-b-c_sqrt(di))/2;
    B_init[0][0] -= lambda;
    B_init[1][1] -= lambda;
    if (c_absval(B_init[0][0]) < RREF_TOL)
    {
        y[0] = 1; y[1] = 0;
    }
    else
    {
        B_init[0][1] /= B_init[0][0];
        b = 1/c_sqrt(1 + B_init[0][1]*B_init[0][1]);
        y[0] = -B_init[0][1]*b;
        y[1] = b;
    }

    /* Compute first p */
    vec_mult_scalar(w, y[1], p, n);
    vec_mult_scalar(Aw, y[1], Ap, n);
    vec_add_scaled(p, x, x, y[0], n);
    vec_add_scaled(Ap, Ax, Ax, y[0], n);
    
    size_t max_iter = 10000; /*TODO: make this a setting */
    for (i = 0; i < max_iter; i++) {

        /* Update w */
        vec_add_scaled(Ax, x, w, -lambda, n);
        /* Note: check inf norm because it is cheaper */
        if (vec_norm_inf(w, n) < LOBPCG_TOL) {
            norm_w = vec_norm_two(w, n);
            lambda -= c_sqrt(2)*norm_w + 1e-6; /* Theoretical bound on the eigenvalue */
            if (n <= 3) lambda -= 1e-6; /* If n <= 3, we should have the exact eigenvalue, hence we subtract a small value */
            break;
        } 
        vec_add_scaled(w, x, w, -vec_prod(x, w, n), n);
        vec_self_mult_scalar(w, 1.0/vec_norm_two(w, n), n);
        mat_vec(A, w_chol, Aw_chol, c);
        xAw = vec_prod(Ax, w, n);
        wAw = vec_prod(w, Aw, n);

        /* Normalize p */
        p_norm_inv = 1.0/vec_norm_two(p, n);
        vec_self_mult_scalar(p, p_norm_inv, n);
        vec_self_mult_scalar(Ap, p_norm_inv, n);

        /* Compress the system */
        xAp = vec_prod(Ax, p, n);
        wAp = vec_prod(Aw, p, n);
        pAp = vec_prod(Ap, p, n);
        xp = vec_prod(x, p, n);
        wp = vec_prod(w, p, n);

        B[0][0] = lambda; B[0][1] = xAw; B[0][2] = xAp; 
        B[1][0] = xAw;    B[1][1] = wAw; B[1][2] = wAp; 
        B[2][0] = xAp;    B[2][1] = wAp; B[2][2] = pAp;

        C[0][2] = xp; C[1][2] = wp; C[2][0] = xp; C[2][1] = wp; 

        /* Solve 3x3 eigenvalue system By = lambda*Cy */
        lambda = custom_eig(B, C, y);

        /* Update p and x */
        vec_mult_add_scaled(p, w, y[2], y[1], n);
        vec_mult_add_scaled(Ap, Aw, y[2], y[1], n);
        vec_mult_add_scaled(x, p, y[0], 1, n);
        vec_mult_add_scaled(Ax, Ap, y[0], 1, n);

        /* Restore consistency between x and lambda once every so often */
        if (mod(i, 50) == 0)
        {
            lambda = vec_prod(x, Ax, n);
        }
    }
    
    //TODO: Implement error handling here
    return lambda;

}
#endif /*QPALM_NONCONVEX*/


void set_settings_nonconvex(QPALMWorkspace *work, solver_common *c){
    #ifdef QPALM_NONCONVEX
    c_float lambda;
    lambda = lobpcg(work, NULL, c);
    if (lambda < 0) {
        work->settings->proximal = TRUE;
        work->settings->gamma_init = 1/c_absval(lambda);
        work->gamma = work->settings->gamma_init;
        work->settings->gamma_max = work->settings->gamma_init;
        work->gamma_maxed = TRUE;
    } else
    {
        work->settings->nonconvex = FALSE;
    }
    #else
    #ifdef QPALM_PRINTING
    qpalm_print("Warning: nonconvex is not supported in this version of QPALM. Setting it to false.\n");
    work->settings->nonconvex = FALSE;
    #endif
    #endif /*QPALM_NONCONVEX*/
}

c_float gershgorin_max(solver_sparse* M, c_float *center, c_float *radius){
    /* NB: Assume M is symmetric, so Gershgorin may be performed along the columns as well. */
    c_float ub_eig = 0.0;
    c_float *Mx = M->x; c_int *Mi = M->i; c_int *Mp = M->p;
    c_int row, i, j, ncol = (c_int)M->ncol;
    
    for (i=0; i < ncol; i++) {
        center[i] = 0.0;
        radius[i] = 0.0;
        for (j = Mp[i]; j < Mp[i+1]; j++) {
            row = Mi[j];
            if (row == i) {
                center[i] = Mx[j];
            } else {
                radius[i] += c_absval(Mx[j]);
            }    
        }
        if (i==0) {
            ub_eig = center[i] + radius[i];
        } else {
            ub_eig = c_max(ub_eig, (center[i] + radius[i]));
        }
    }

    return ub_eig;
}

