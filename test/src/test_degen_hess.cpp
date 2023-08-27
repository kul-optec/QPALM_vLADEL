#include "minunit.h"
#include <qpalm.h>

#define N 3
#define M 4
#define ANZMAX 6
#define QNZMAX 4
#define TOL 1e-5

static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
static QPALMData *data;
static solver_common *c;
static solver_common common;


void degen_hess_suite_setup(void) {
    settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-6;
    settings->eps_rel = 1e-6;

    data = (QPALMData *)qpalm_malloc(sizeof(QPALMData));
    data->n = N;
    data->m = M;
    data->c = 0;
    data->q = (c_float *)qpalm_calloc(N,sizeof(c_float));
    data->q[0] = -2; data->q[1] = -6; data->q[2] = 1;  
    data->bmin = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmin[0] = 0.5; data->bmin[1] = -10; data->bmin[2] = -10; data->bmin[3] = -10; 
    data->bmax = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmax[0] = 0.5; data->bmax[1] = 10; data->bmax[2] = 10; data->bmax[3] = 10;

    // solver_common common;
    c = &common;
    solver_sparse *A = ladel_sparse_alloc(M, N, ANZMAX, UNSYMMETRIC, TRUE, FALSE);
    solver_sparse *Q = ladel_sparse_alloc(N, N, QNZMAX, UPPER, TRUE, FALSE);

    c_float *Ax;
    c_int *Ai, *Ap;
    Ax = A->x;
    Ap = A->p;
    Ai = A->i;
    Ax[0] = 1.0; Ax[1] = 1.0; Ax[2] = 1.0; Ax[3] = 1.0; Ax[4] = 1.0; Ax[5] = 1.0;
    Ap[0] = 0; Ap[1] = 2; Ap[2] = 4; Ap[3] = 6;
    Ai[0] = 0; Ai[1] = 1; Ai[2] = 0; Ai[3] = 2; Ai[4] = 0; Ai[5] = 3;

    c_float *Qx;
    c_int *Qi, *Qp;
    Qx = Q->x;
    Qp = Q->p;
    Qi = Q->i;
    Qx[0] = 1.0; Qx[1] = -1.0; Qx[2] = -1.0; Qx[3] = 2.0;  
    Qp[0] = 0; Qp[1] = 2; Qp[2] = 4; Qp[3] = 4;
    Qi[0] = 0; Qi[1] = 1; Qi[2] = 0; Qi[3] = 1; 

    ladel_to_upper_diag(Q);

    data->A = A;
    data->Q = Q;
}

void degen_hess_suite_teardown(void) {
    qpalm_free(settings);
    // Clean setup
    data->Q = ladel_sparse_free(data->Q);
    data->A = ladel_sparse_free(data->A);
    qpalm_free(data->q);
    qpalm_free(data->bmin);
    qpalm_free(data->bmax);
    qpalm_free(data);
}

void degen_hess_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestDegenHessQP : ::testing::TestWithParam<int> {
    void SetUp() override {
        degen_hess_suite_setup();
        // degen_hess_test_setup();
        settings->max_rank_update_fraction = 1.0;
        settings->factorization_method = GetParam();
    }
    void TearDown() override {
        degen_hess_test_teardown();
        degen_hess_suite_teardown();
    }
};

TEST_P(TestDegenHessQP, test_degen_hess) {
    // Setup workspace
    work = qpalm_setup(data, settings);
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);
    mu_assert_double_eq(work->solution->x[0], 5.5, TOL);
    mu_assert_double_eq(work->solution->x[1], 5, TOL);
    mu_assert_double_eq(work->solution->x[2], -10, TOL);
}

INSTANTIATE_TEST_SUITE_P(TestDegenHessQP, TestDegenHessQP,
                         ::testing::Values(FACTORIZE_KKT_OR_SCHUR, 
                                           FACTORIZE_KKT,
                                           FACTORIZE_SCHUR));
