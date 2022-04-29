#include "qpalm.h"
#include <stdio.h>

#define N 2
#define M 3
#define ANZMAX 4
#define QNZMAX 2

#define TRUE 1
#define FALSE 0

c_float* random_vector(c_int n) {
  c_float* X = qpalm_calloc(n, sizeof(c_float));
  for (int i = 0; i < n; i++) {
    X[i] = (c_float) 10*rand()/(c_float) RAND_MAX;
  }
  return X;
}

c_float* constant_vector(c_float c, c_int n) {
  c_float* X = qpalm_calloc(n, sizeof(c_float));
  for (int i = 0; i < n; i++) {
    X[i] = c;
  }
  return X;
}
int main() {

// Load problem data

  c_int n = N;
  c_int m = M;

  // Problem settings
  QPALMSettings *settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));

  // Structures
  QPALMWorkspace *work; // Workspace
  QPALMData *data;      // QPALMData

  // Populate data
  data    = (QPALMData *)qpalm_malloc(sizeof(QPALMData));
  data->n = n;
  data->m = m;
  data->q = random_vector(data->n);
  data->c = 0;
  data->bmin = constant_vector(-2, data->m);
  data->bmax = constant_vector(2, data->m);

  solver_sparse *A, *Q;
  A = ladel_sparse_alloc(M, N, ANZMAX, UNSYMMETRIC, TRUE, FALSE);
  Q = ladel_sparse_alloc(N, N, QNZMAX, UPPER, TRUE, FALSE);

  c_float *Ax;
  c_int *Ai, *Ap;
  Ax = A->x;
  Ap = A->p;
  Ai = A->i;
  Ax[0] = 1.0; Ax[1] = 1.0; Ax[2] = 1.0; Ax[3] = 1.0;
  Ap[0] = 0; Ap[1] = 2; Ap[2] = 4;
  Ai[0] = 0; Ai[1] = 2; Ai[2] = 1; Ai[3] = 2;
  c_float *Qx;
  c_int *Qi, *Qp;
  Qx = Q->x;
  Qp = Q->p;
  Qi = Q->i;
  Qx[0] = 1.0; Qx[1] = 1.5; 
  Qp[0] = 0; Qp[1] = 1; Qp[2] = 2;
  Qi[0] = 0; Qi[1] = 1; 

  data->A = A;
  data->Q = Q;

  // Define Solver settings as default
  qpalm_set_default_settings(settings);

  // Setup workspace
  work = qpalm_setup(data, settings);

  // Solve Problem
  qpalm_solve(work);

  printf("Solver status: ");
  puts(work->info->status);
  printf("Iter: %d\n", (int) work->info->iter);
  printf("Iter Out: %d\n", (int) work->info->iter_out);
  // for (c_int i = 0; i < n; i++) {
  //   printf("Solution variable %.10f \n",work->x[i]);
  // }
  #ifdef PROFILING
  printf("Setup time: %f\n", work->info->setup_time);
  printf("Solve time: %f\n", work->info->solve_time);
  printf("Run time: %f\n", work->info->run_time);
  #endif

  // Clean workspace
  data->Q = ladel_sparse_free(data->Q);
  data->A = ladel_sparse_free(data->A);

  qpalm_cleanup(work);
  qpalm_free(data->q);
  qpalm_free(data->bmin);
  qpalm_free(data->bmax);
  qpalm_free(data);
  qpalm_free(settings);

  return 0;

}