/**
 * @file solver_interface.c
 * @author Ben Hermans
 * @brief Interface and wrapper to matrix/factorization (ladel) functions
 * @details This file includes all calls to ladel functions apart from scaling in scaling.c and memory
 * allocation/deallocation in the main functions in qpalm.c. It includes all matrix operations, such as
 * matrix vector products, row- and columnwise norms, cholesky factorizations, factorization updates and
 * solving the linear system. 
 * 
 */

#include "lin_alg.h"
#include "solver_interface.h"
#include <stdio.h>

#include "ladel.h"

void qpalm_set_factorization_method(QPALMWorkspace *work, solver_common *c)
{
  if (work->settings->factorization_method == FACTORIZE_KKT_OR_SCHUR)
  {
    ladel_int col, index, nnz_kkt, nnz_schur, n = work->data->n, m = work->data->m;
    solver_sparse *Q = work->data->Q, *A = work->data->A;
    /* Compute nnz in KKT */
    nnz_kkt = Q->nzmax + n + A->nzmax + m;
    /* Compensate for diagonal entries present in Q */ 
    for (col = 1; col <= n; col++)
    {
      index = Q->p[col]-1;
      if (index >= 0 && Q->i[index] == col-1) nnz_kkt--;
    } 

    /* Compute nnz in SCHUR (approximately) */
    nnz_schur = nnz_kkt - A->nzmax - m; /* Q + diagonal */

    /* (over)estimate the nnz introduced by AtA */
    solver_sparse *At;
    c->array_int_ncol1 = work->index_L; /* Avoid allocating full workspace */
    At = ladel_transpose(work->data->A, FALSE, c);
    c->array_int_ncol1 = NULL;

    ladel_int Atnz_col, Atnz_col_max = 0;
    for (col = 0; col < m; col++) Atnz_col_max = c_max(At->p[col+1] - At->p[col], Atnz_col_max);
    for (col = 0; col < m; col++)
    {
      Atnz_col = At->p[col+1] - At->p[col];
      if (Atnz_col + Atnz_col_max <= n)
        nnz_schur += (Atnz_col*(Atnz_col-1))/2;
      else
		  nnz_schur += (Atnz_col*(n - Atnz_col_max) - ((n - Atnz_col_max)*(n - Atnz_col_max + 1)) / 2);
    }
	if (2 * Atnz_col_max > n) nnz_schur += (Atnz_col_max*(Atnz_col_max - 1)) / 2 - (Atnz_col_max*(n - Atnz_col_max) - ((n - Atnz_col_max)*(n - Atnz_col_max + 1)) / 2);
    nnz_schur = c_max(c_min(nnz_schur, (n*(n-1))/2), 1);
    /* NB: If nnz(KKT) == nnz(Q+AtA) << n^2, then KKT will perform a bit better due to the 
    ordering. */;
    At = ladel_sparse_free(At);

    /* Switching criterion */
    if ((nnz_kkt*nnz_kkt)/(nnz_schur*nnz_schur)*n/(n+m) < 2) 
        work->solver->factorization_method = FACTORIZE_KKT;
    else
        work->solver->factorization_method = FACTORIZE_SCHUR;

  } else
  {
    work->solver->factorization_method = work->settings->factorization_method;
  }

}



void mat_vec(solver_sparse *A, solver_dense *x, solver_dense *y, solver_common *c) 
{
    (void)c;
    ladel_int n = A->ncol;
    if (x!=y) {
      if (A->symmetry == UNSYMMETRIC)
        ladel_matvec(A, x, y, TRUE);
      else
        ladel_symmetric_matvec(A, x, y, TRUE);
    } else {
      ladel_double* x2 = ladel_malloc(n, sizeof(c_float));
      ladel_double_vector_copy(x, n, x2);
      if (A->symmetry == UNSYMMETRIC)
        ladel_matvec(A, x2, y, TRUE);
      else
        ladel_symmetric_matvec(A, x2, y, TRUE);
      ladel_free(x2);
    }
}

void mat_tpose_vec(solver_sparse *A, solver_dense *x, solver_dense *y, solver_common *c)
{
    (void)c;
    ladel_int m = A->nrow;
    if (x!=y) {
      if (A->symmetry == UNSYMMETRIC)
        ladel_tpose_matvec(A, x, y, TRUE);
      else
        ladel_symmetric_matvec(A, x, y, TRUE);  
    } else {
      ladel_double* x2 = ladel_malloc(m, sizeof(c_float));
      ladel_double_vector_copy(x, m, x2);
      if (A->symmetry == UNSYMMETRIC)
        ladel_tpose_matvec(A, x2, y, TRUE);
      else
        ladel_symmetric_matvec(A, x2, y, TRUE); 
      ladel_free(x2);
    }
}

// TODO: incorporate the factorizations here also, and write a version of FACTORIZE_KKT using cholmod 
void qpalm_form_kkt(QPALMWorkspace *work)
{
    solver_sparse *Q = work->data->Q, *kkt = work->solver->kkt, *kkt_full = work->solver->kkt_full, *At = work->solver->At;
    ladel_int col, index, index_kkt, n = work->data->n, m = work->data->m, Qnz = Q->nzmax;
    c_float *sigma_inv = work->sigma_inv, *first_elem_A = work->solver->first_elem_A;
    c_int *first_row_A = work->solver->first_row_A;
    /* copy Q */
    for (col = 0; col < n; col++)
    {
        kkt->p[col] = kkt_full->p[col] = Q->p[col];
        kkt->nz[col] = Q->p[col+1] - Q->p[col];
    }
    kkt->p[col] = kkt_full->p[col] = Q->p[col];
    for (index = 0; index < Qnz; index++)
    {
        kkt->i[index] = kkt_full->i[index] = Q->i[index];
        kkt->x[index] = kkt_full->x[index] = Q->x[index];
    }

    /* copy [At; -\Sigma^{-1}] */
    index_kkt = Qnz;
    for (; col < m+n; col++)
    {
        kkt_full->i[index_kkt] = first_row_A[col-n] = At->i[At->p[col-n]];
        kkt_full->x[index_kkt] = first_elem_A[col-n] = At->x[At->p[col-n]];

        if (work->solver->active_constraints[col-n])
        {
            kkt->nz[col] = At->p[col-n+1] - At->p[col-n] + 1;
            kkt->i[index_kkt] = At->i[At->p[col-n]];
            kkt->x[index_kkt] = At->x[At->p[col-n]];
        } 
        else 
        {
            kkt->nz[col] = 1;
            kkt->i[index_kkt] = col;
            kkt->x[index_kkt] = 1;
        }

        if (At->p[col-n+1]-At->p[col-n] != 0) index_kkt++;

        for (index = At->p[col-n]+1; index < At->p[col-n+1]; index++)
        {
            kkt->i[index_kkt] = kkt_full->i[index_kkt] = At->i[index];
            kkt->x[index_kkt] = kkt_full->x[index_kkt] = At->x[index];
            index_kkt++;
        }

        kkt->i[index_kkt] = kkt_full->i[index_kkt] = col;
        kkt->x[index_kkt] = kkt_full->x[index_kkt] = -sigma_inv[col-n];
        if (At->p[col-n+1]-At->p[col-n] == 0) kkt->x[index_kkt] = 1;
        index_kkt++;

        kkt->p[col+1] = kkt_full->p[col+1] = Qnz + At->p[col+1-n] + 1 + col - n;
    }
}


void qpalm_reform_kkt(QPALMWorkspace *work)
{
    solver_sparse *kkt = work->solver->kkt, *At = work->solver->At;
    ladel_int col, n = work->data->n, m = work->data->m;
    ladel_int *first_row_A = work->solver->first_row_A;
    ladel_double *sigma_inv = work->sigma_inv, *first_elem_A = work->solver->first_elem_A;

    for (col = n; col < n+m; col++)
    {
        if (work->solver->active_constraints[col-n])
        {
            kkt->nz[col] = At->p[col-n+1] - At->p[col-n] + 1;
            kkt->i[kkt->p[col]] = first_row_A[col-n];
            kkt->x[kkt->p[col]] = first_elem_A[col-n];
            kkt->x[kkt->p[col+1]-1] = -sigma_inv[col-n]; /* row index should be correct already */
            kkt->i[kkt->p[col+1]-1] = col;
        } else
        {
            kkt->nz[col] = 1;
            kkt->i[kkt->p[col]] = col;
            kkt->x[kkt->p[col]] = 1; 
        }
    }
}

void kkt_update_entering_constraints(QPALMWorkspace *work, solver_common *c)
{
    solver_sparse *kkt = work->solver->kkt, *At = work->solver->At;
    ladel_int col, index, n = work->data->n;
    ladel_int *first_row_A = work->solver->first_row_A;
    ladel_double *sigma_inv = work->sigma_inv, *first_elem_A = work->solver->first_elem_A;

    for (index = 0; index < work->solver->nb_enter; index++)
    {
        col = work->solver->enter[index] + n;
        kkt->nz[col] = At->p[col-n+1] - At->p[col-n] + 1;
        kkt->i[kkt->p[col]] = first_row_A[col-n];
        kkt->x[kkt->p[col]] = first_elem_A[col-n];
        kkt->x[kkt->p[col+1]-1] = -sigma_inv[col-n]; /* row index should be correct already */
        ladel_row_add(work->solver->LD, work->solver->sym, col, kkt, col, -sigma_inv[col-n], c);
    }
}

void kkt_update_leaving_constraints(QPALMWorkspace *work, solver_common *c)
{
    ladel_int col, index, n = work->data->n;
    ladel_double *sigma_inv = work->sigma_inv;
    solver_sparse *kkt = work->solver->kkt;

    for (index = 0; index < work->solver->nb_leave; index++)
    {
        col = work->solver->leave[index] + n;
        ladel_row_del(work->solver->LD, work->solver->sym, col, c);
        
        /* no need to update the kkt system here, except for iterative refinement */
        kkt->nz[col] = 1;
        kkt->i[kkt->p[col]] = col;
        kkt->x[kkt->p[col]] = -sigma_inv[col-n];
    }
}

void kkt_solve(QPALMWorkspace *work, solver_common *c)
{
    c_int n = work->data->n, m = work->data->m;
    prea_vec_copy(work->dphi, work->solver->rhs_kkt, n);
    vec_self_mult_scalar(work->solver->rhs_kkt, -1, n);
    vec_set_scalar(work->solver->rhs_kkt + n, 0, m);

    ladel_dense_solve(work->solver->LD, work->solver->rhs_kkt, work->solver->sol_kkt, c);
    prea_vec_copy(work->solver->sol_kkt, work->d, n);
}



void ldlchol(solver_sparse *M, QPALMWorkspace *work, solver_common *c) {
  ladel_diag d;
  d.diag_elem = 1.0/work->gamma;
  if (work->settings->proximal) d.diag_size = work->data->n;
  else d.diag_size = 0;

  if (work->solver->first_factorization)
  {
    work->solver->LD = ladel_factor_free(work->solver->LD);
    /* Compute the pattern of Q+A^T*A to allocate L */
    solver_sparse *AtA, *QAtA;
    AtA = ladel_mat_mat_transpose_pattern(work->solver->At_sqrt_sigma, work->data->A, c);
    QAtA = ladel_add_matrices_pattern(work->data->Q, AtA, c);
    QAtA->symmetry = UPPER;

    /* TODO: consider SCHUR method also with ordering */
    ladel_factorize_advanced_with_diag(M, d, work->solver->sym, NO_ORDERING, &work->solver->LD, QAtA, c);
    
    ladel_sparse_free(AtA);
    ladel_sparse_free(QAtA);
    work->solver->first_factorization = FALSE;
  }
  else
  {
    ladel_factorize_with_prior_basis_with_diag(M, d, work->solver->sym, work->solver->LD, c);
  }
}

void ldlcholQAtsigmaA(QPALMWorkspace *work, solver_common *c) {
  solver_sparse *AtsigmaA;
  solver_sparse *QAtsigmaA;
  size_t nb_active = 0;
  for (size_t i = 0; i < work->data->m; i++) {
      if (work->solver->active_constraints[i]){
          work->solver->enter[nb_active] = (c_int)i;
          nb_active++;
      }      
  }
  solver_sparse *At_sqrt_sigma = ladel_column_submatrix(work->solver->At_sqrt_sigma, work->solver->enter, nb_active);
  solver_sparse *A_sqrt_sigma = ladel_transpose(At_sqrt_sigma, TRUE, c);
  AtsigmaA = ladel_mat_mat_transpose(At_sqrt_sigma, A_sqrt_sigma, c);
  QAtsigmaA = ladel_add_matrices(1.0, work->data->Q, 1.0, AtsigmaA, c);
  QAtsigmaA->symmetry = UPPER;
  ldlchol(QAtsigmaA, work, c);

  AtsigmaA = ladel_sparse_free(AtsigmaA);
  QAtsigmaA = ladel_sparse_free(QAtsigmaA);
  At_sqrt_sigma = ladel_sparse_free(At_sqrt_sigma);
  A_sqrt_sigma = ladel_sparse_free(A_sqrt_sigma);
}

void ldlupdate_entering_constraints(QPALMWorkspace *work, solver_common *c) {
  ladel_int index;
  for (index = 0; index < work->solver->nb_enter; index++)
  {
    ladel_rank1_update(work->solver->LD, work->solver->sym, work->solver->At_sqrt_sigma, 
                        work->solver->enter[index], 1.0, UPDATE, c);
  }
}

void ldldowndate_leaving_constraints(QPALMWorkspace *work, solver_common *c) {
  ladel_int index;
  for (index = 0; index < work->solver->nb_leave; index++)
  {
    ladel_rank1_update(work->solver->LD, work->solver->sym, work->solver->At_sqrt_sigma, 
                        work->solver->leave[index], 1.0, DOWNDATE, c);
  }
}

void ldlupdate_sigma_changed(QPALMWorkspace *work, solver_common *c) {
  c_int *sigma_changed = work->solver->enter;
  c_int row;
  size_t k, nb_sigma_changed = (size_t) work->nb_sigma_changed;
  
  c_float *At_scalex = work->solver->At_scale;
  
  for (k = 0; k < nb_sigma_changed; k++) {
    row = sigma_changed[k];
    At_scalex[row] = At_scalex[row]*At_scalex[row];
    if (work->solver->factorization_method == FACTORIZE_SCHUR) 
      At_scalex[row] = c_sqrt(1-1/At_scalex[row]); 
  }

  if (work->solver->factorization_method == FACTORIZE_KKT)
  {
    ladel_sparse_matrix *W = ladel_sparse_alloc(work->data->n + work->data->m, 1, 1, UNSYMMETRIC, TRUE, FALSE);
    W->p[0] = 0;
    W->p[1] = 1;
    W->x[0] = 1.0;
    c_int row;
    c_float factor;
    for (k = 0; k < nb_sigma_changed; k++)
    {
      row = sigma_changed[k];
      W->i[0] = (work->solver->LD->pinv) ? work->solver->LD->pinv[row] : row;
      // ladel_print("Row: %d\n", row);
      factor = work->sigma_inv[row]*(At_scalex[row] - 1.0);
      // ladel_print("Factor: %f\n", factor);
      ladel_rank1_update(work->solver->LD, work->solver->sym, W, 0, factor, UPDATE, c);
    }
    ladel_sparse_free(W);
    work->solver->reset_newton = TRUE;
  } else
  {
    for (k = 0; k < nb_sigma_changed; k++)
    {
      ladel_rank1_update(work->solver->LD, work->solver->sym, work->solver->At_sqrt_sigma, 
                          sigma_changed[k], At_scalex[sigma_changed[k]], UPDATE, c);
    }
  }
  
}

void ldlsolveLD_neg_dphi(QPALMWorkspace *work, solver_common *c) {
  //set -dphi
  prea_vec_copy(work->dphi, work->neg_dphi, work->data->n);
  vec_self_mult_scalar(work->neg_dphi, -1, work->data->n);
  ladel_dense_solve(work->solver->LD, work->neg_dphi, work->d, c);
}

