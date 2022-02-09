#include "minunit.h"
#include "qpalm.h"
#include <gtest/gtest.h>

#define N 2
#define M 3
#define ANZMAX 4
#define QNZMAX 2

// Structures
static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
static QPALMData *data;
static solver_common *c;
static solver_common common;


void prim_inf_qp_suite_setup(void) {
    settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));

    data = (QPALMData *)qpalm_malloc(sizeof(QPALMData));
    data->n = N;
    data->m = M;
    data->c = 0;

    data->q = (c_float *)qpalm_calloc(N,sizeof(c_float));
    data->q[0] = 1; data->q[1] = -2; 
    data->bmin = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmin[0] = -5; data->bmin[1] = -10; data->bmin[2] = 16; 
    data->bmax = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmax[0] = 5; data->bmax[1] = 10; data->bmax[2] = 20; 

    c = &common;
    solver_sparse *A = ladel_sparse_alloc(M, N, ANZMAX, UNSYMMETRIC, TRUE, FALSE);
    solver_sparse *Q = ladel_sparse_alloc(N, N, QNZMAX, UPPER, TRUE, FALSE);
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
}

void prim_inf_qp_suite_teardown(void) {
    qpalm_free(settings);
    // Clean setup
    data->Q = ladel_sparse_free(data->Q);
    data->A = ladel_sparse_free(data->A);
    qpalm_free(data->q);
    qpalm_free(data->bmin);
    qpalm_free(data->bmax);
    qpalm_free(data);
}

void prim_inf_qp_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestPrimInfQP : ::testing::TestWithParam<int> {
    void SetUp() override {
        prim_inf_qp_suite_setup();
        // prim_inf_qp_test_setup();
        qpalm_set_default_settings(settings);
        settings->eps_abs = 1e-6;
        settings->eps_rel = 1e-6;
        settings->max_rank_update_fraction = 1.0;
        settings->factorization_method = GetParam();
    }
    void TearDown() override {
        prim_inf_qp_test_teardown();
        prim_inf_qp_suite_teardown();
    }
};

TEST_P(TestPrimInfQP, test_prim_inf_qp) {
    settings->proximal = TRUE;
    settings->scaling = 2;
    // Setup workspace
    work = qpalm_setup(data, settings);
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_PRIMAL_INFEASIBLE);
}

TEST_P(TestPrimInfQP, test_prim_inf_qp_unscaled) {
    settings->proximal = TRUE;
    settings->scaling = 0;
    // Setup workspace
    work = qpalm_setup(data, settings);
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_PRIMAL_INFEASIBLE);
}

TEST_P(TestPrimInfQP, test_prim_inf_qp_noprox) {
    settings->proximal = FALSE;
    settings->scaling = 2;
    // Setup workspace
    work = qpalm_setup(data, settings);
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_PRIMAL_INFEASIBLE);
}

TEST_P(TestPrimInfQP, test_prim_inf_qp_noprox_unscaled) {
    settings->proximal = FALSE;
    settings->scaling = 0;
    // Setup workspace
    work = qpalm_setup(data, settings);
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_PRIMAL_INFEASIBLE);
}

INSTANTIATE_TEST_SUITE_P(TestPrimInfQP, TestPrimInfQP,
                         ::testing::Values(FACTORIZE_KKT_OR_SCHUR, 
                                           FACTORIZE_KKT,
                                           FACTORIZE_SCHUR));
