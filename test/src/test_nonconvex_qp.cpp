#ifdef QPALM_NONCONVEX

#include "minunit.h"
#include <qpalm.h>

#define N 4
#define M 5
#define ANZMAX 4
#define QNZMAX 4

static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
static QPALMData *data;
static solver_common *c;
static solver_common common;

void nonconvex_qp_suite_setup(void) {
    settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-6;
    settings->eps_rel = 1e-6;
    settings->nonconvex = TRUE;
    settings->scaling = FALSE; /*So we can retrieve the actual eigenvalue*/
 
    data = (QPALMData *)qpalm_malloc(sizeof(QPALMData));
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
    Ai[0] = 3;
    Ax[1] = 0.0254311360000000;
    Ai[1] = 4;
    Ax[2] = -0.0001000000000000;
    Ai[2] = 0;
    Ax[3] = 0.3306698500000000;
    Ai[3] = 2;
    Ap[0] = 0;
    Ap[1] = 1;
    Ap[2] = 2;
    Ap[3] = 3;
    Ap[4] = 4;

    c_float *Qx = data->Q->x;
    c_int *Qi = data->Q->i;
    c_int *Qp = data->Q->p;
    Qx[0] = 1.0000000000000000;
    Qi[0] = 0;
    Qx[1] = 0.0464158880000000;
    Qi[1] = 1;
    Qx[2] = -0.0021544347000000;
    Qi[2] = 2;
    Qx[3] = 0.0001000000000000;
    Qi[3] = 3;
    Qp[0] = 0;
    Qp[1] = 1;
    Qp[2] = 2;
    Qp[3] = 3;
    Qp[4] = 4;

    data->q = (c_float *)qpalm_calloc(N,sizeof(c_float));
    data->bmin = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmax = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmin[0] = -2; 
    data->bmin[1] = -2;
    data->bmin[2] = -2;
    data->bmin[3] = -2;
    data->bmin[4] = -2;

    data->bmax[0] = 2; 
    data->bmax[1] = 2;
    data->bmax[2] = 2;
    data->bmax[3] = 2;
    data->bmax[4] = 2;

    data->q[0] = -2.0146781e+00; 
    data->q[1] = 2.9613971e+00; 
    data->q[2] = 7.2865370e+00; 
    data->q[3] = 7.8925204e+00;   
}

void nonconvex_qp_suite_teardown(void) {
    // Cleanup temporary structures
    qpalm_free(settings);
    data->Q = ladel_sparse_free(data->Q);
    data->A = ladel_sparse_free(data->A);

    qpalm_free(data->q);
    qpalm_free(data->bmin);
    qpalm_free(data->bmax);
    qpalm_free(data);
}

void nonconvex_qp_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestNonconvexQP : ::testing::TestWithParam<int> {
    void SetUp() override {
        nonconvex_qp_suite_setup();
        // nonconvex_qp_test_setup();
        settings->max_rank_update_fraction = 1.0;
        settings->factorization_method = GetParam();
    }
    void TearDown() override {
        nonconvex_qp_test_teardown();
        nonconvex_qp_suite_teardown();
    }
};

TEST_P(TestNonconvexQP, test_nonconvex_qp) {
    // Setup workspace
    work = qpalm_setup(data, settings); 
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);
    mu_assert_double_eq(work->gamma, 1.0/0.0021544347000000, 1e-1*1.0/0.0021544347000000); /* inverse of the lowest eigenvalue */
    mu_assert_true(1/work->gamma > 0.0021544347000000); /* eigenvalue is underapproximated (here negative signs were dropped) */
}

INSTANTIATE_TEST_SUITE_P(TestNonconvexQP, TestNonconvexQP,
                         ::testing::Values(FACTORIZE_KKT_OR_SCHUR, 
                                           FACTORIZE_KKT,
                                           FACTORIZE_SCHUR));

#endif // QPALM_NONCONVEX