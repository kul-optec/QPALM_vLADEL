#include "minunit.h"
#include "qpalm.h"

#define N 2
#define M 2
#define ANZMAX 2
#define QNZMAX 2

static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
static QPALMData *data;
static solver_common *c;
static solver_common common;

/* Problem to exercise the linesearch when all breakpoints are traversed. */

void ls_qp_suite_setup(void) {
    settings = (QPALMSettings *)c_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-6;
    settings->eps_rel = 1e-6;
    settings->gamma_max = 1e3;
    settings->gamma_init = 1e1;

    data = (QPALMData *)c_malloc(sizeof(QPALMData));
    data->n = N;
    data->m = M;
    data->c = 0;

    c = &common;
    data->A = ladel_sparse_alloc(M, N, ANZMAX, UNSYMMETRIC, TRUE, FALSE);
    data->Q = ladel_sparse_alloc(N, N, QNZMAX, UPPER, TRUE, FALSE);

    c_float *Ax = data->A->x;
    c_int *Ai = data->A->i;
    c_int *Ap = data->A->p;
    Ax[0] = -1.0000000000000000;
    Ai[0] = 1;
    Ax[1] = 0.0001000000000000;
    Ai[1] = 0;
    Ap[0] = 0;
    Ap[1] = 1;
    Ap[2] = 2;

    c_float *Qx = data->Q->x;
    c_int *Qi = data->Q->i;
    c_int *Qp = data->Q->p;
    Qx[0] = 1.0000000000000000;
    Qi[0] = 0;
    Qx[1] = 0.0001000000000000;
    Qi[1] = 1;
    Qp[0] = 0;
    Qp[1] = 1;
    Qp[2] = 2;

    data->q = (c_float *)c_calloc(N,sizeof(c_float));
    data->bmin = (c_float *)c_calloc(M,sizeof(c_float));
    data->bmax = (c_float *)c_calloc(M,sizeof(c_float));
    data->bmin[0] = -2; data->bmin[1] = -2;
    data->bmax[0] = 2; data->bmax[1] = 2;
    data->q[0] = 2.5150105e+00; data->q[1] = 1.6259589e+01;
}

void ls_qp_suite_teardown(void) {
    c_free(settings);
    // Clean setup
    data->Q = ladel_sparse_free(data->Q);
    data->A = ladel_sparse_free(data->A);
    c_free(data->q);
    c_free(data->bmin);
    c_free(data->bmax);
    c_free(data);
}

void ls_qp_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestLSQP : ::testing::Test {
    void SetUp() override {
        ls_qp_suite_setup();
        // ls_qp_test_setup();
    }
    void TearDown() override {
        ls_qp_test_teardown();
        ls_qp_suite_teardown();
    }

    void test_ls_qp() {
        // Setup workspace
        work = qpalm_setup(data, settings);
        // Solve Problem
        qpalm_solve(work);

        mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);

        c_float *solution = (c_float *)c_calloc(N,sizeof(c_float));
        solution[0] = -2.0000000e+00;
        solution[1] = -2.0000000e+04;
        

        for(c_int i = 0; i < N; i++) {
            mu_assert_double_eq(work->solution->x[i], solution[i], 1e-5);
        }

        c_free(solution);
    }
};

TEST_F(TestLSQP, testLSQPKKTorSchur) {
    settings->max_rank_update_fraction = 1.0;
    settings->factorization_method = FACTORIZE_KKT_OR_SCHUR;
    test_ls_qp();
}

TEST_F(TestLSQP, testLSQPKKT) {
    settings->max_rank_update_fraction = 1.0;
    settings->factorization_method = FACTORIZE_KKT;
    test_ls_qp();
}

TEST_F(TestLSQP, testLSQPSchur) {
    settings->max_rank_update_fraction = 1.0;
    settings->factorization_method = FACTORIZE_SCHUR;
    test_ls_qp();
}
