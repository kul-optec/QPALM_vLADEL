#include "minunit.h"
#include <qpalm.h>

#define N 5
#define M 7
#define ANZMAX 14
#define QNZMAX 7
#define TOL 1e-6

static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
// QPALMData *data;
static c_float *w;
static solver_common *c;
static solver_common common;

void casadi_general_convex_sparse_suite_setup(void) {
    settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-12;
    settings->eps_rel = 1e-12;
    // settings->scaling = 0;

    c_float* dummy = (c_float *)qpalm_calloc(ANZMAX+QNZMAX, sizeof(c_float)); dummy[0] = 0; dummy[1] = 0;
    c_int *Ai = (c_int *)qpalm_calloc(ANZMAX, sizeof(c_int)); 
    c_int *Ap = (c_int *)qpalm_calloc(N+1, sizeof(c_int));
    Ap[0] = 0; Ap[1] = 3; Ap[2] = 5; Ap[3] = 8; Ap[4] = 11; Ap[5] = 14; 
    Ai[0] = 0; Ai[1] = 5; Ai[2] = 6; Ai[3] = 1; Ai[4] = 6; Ai[5] = 2; Ai[6] = 5; Ai[7] = 6; Ai[8] = 3; Ai[9] = 5; Ai[10] = 6; Ai[11] = 4; Ai[12] = 5; Ai[13] = 6;   

    ladel_sparse_matrix A;
    A.nrow = M;
    A.ncol = N;
    A.nz = NULL;
    A.nzmax = ANZMAX;
    A.x = dummy;
    A.i = Ai;
    A.p = Ap;
    A.symmetry = UNSYMMETRIC;
    A.values = TRUE;

    c_int *Qi = (c_int *)qpalm_calloc(QNZMAX, sizeof(c_int)); 
    c_int *Qp = (c_int *)qpalm_calloc(N+1, sizeof(c_int));
    Qp[0] = 0; Qp[1] = 1; Qp[2] = 3; Qp[3] = 5; Qp[4] = 6; Qp[5] = 7; 
    Qi[0] = 0; Qi[1] = 1; Qi[2] = 2; Qi[3] = 1; Qi[4] = 2; Qi[5] = 3; Qi[6] = 4;  

    ladel_sparse_matrix Q;
    Q.nrow = N;
    Q.ncol = N;
    Q.nz = NULL;
    Q.nzmax = QNZMAX;
    Q.x = dummy;
    Q.i = Qi;
    Q.p = Qp;
    Q.symmetry = UPPER;
    Q.values = TRUE;

    QPALMData data;
    // Populate data
    data.n = N;
    data.m = M;
    // csc_matrix in mem
    data.Q = &Q;
    data.q = dummy;
    data.c = 0;
    data.A = &A;
    data.bmin = dummy;
    data.bmax = dummy;

    // Setup workspace
    work = qpalm_setup(&data, settings);

    qpalm_free(dummy);
    qpalm_free(Ai);
    qpalm_free(Ap);
    qpalm_free(Qi);
    qpalm_free(Qp);

    w = (c_float *)qpalm_calloc(QNZMAX+ANZMAX, sizeof(c_float)); //double workspace
}

void casadi_general_convex_sparse_suite_teardown(void) {
    qpalm_free(settings);
    // Clean setup
    qpalm_free(w);
}

void casadi_general_convex_sparse_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestCasadiGeneralConvexSparse : ::testing::Test {
    void SetUp() override {
        casadi_general_convex_sparse_suite_setup();
        // casadi_general_convex_sparse_test_setup();
    }
    void TearDown() override {
        casadi_general_convex_sparse_test_teardown();
        casadi_general_convex_sparse_suite_teardown();
    }
};

TEST_F(TestCasadiGeneralConvexSparse, test_casadi_general_convex_sparse) {
    // Setup workspace
    // work = qpalm_setup(data, settings);
    c_float *Qx = w, *Ax = w+QNZMAX;
    //casadi_tri_project should have filtered out lower diagonal entries
    Qx[0] = 2; Qx[1] = 1; Qx[2] = 0.1; Qx[3] = 0.2; Qx[4] = 0.7; Qx[5] = 1.3;  
    Ax[0] = 1; Ax[1] = 1; Ax[2] = 0.1; Ax[3] = 1; Ax[4] = 2; Ax[5] = 1; Ax[6] = 0.1; Ax[7] = -0.3; Ax[8] = 1; Ax[9] = 0.7; Ax[10] = 4; Ax[11] = 1; Ax[12] = -1; Ax[13] = 0.1; 
    qpalm_update_Q_A(work, Qx, Ax);

    // ladel_print_sparse_matrix_matlab(work->data->Q);
    // ladel_print_sparse_matrix_matlab(work->data->A);

    c_float *q = w;
    q[0] = -2; q[1] = -6; q[2] = 1; q[3] = 0; q[4] = 0;

    qpalm_update_q(work, q);

    c_float *bmin = w, *bmax = w+M;
    bmin[0] = 0; bmin[1] = 0; bmin[2] = 0; bmin[3] = 0; bmin[4] = 0; bmin[5] = -QPALM_INFTY; bmin[6] = -QPALM_INFTY; 
    bmax[0] = QPALM_INFTY; bmax[1] = QPALM_INFTY; bmax[2] = QPALM_INFTY; bmax[3] = QPALM_INFTY; bmax[4] = QPALM_INFTY; bmax[5] = 2; bmax[6] = 2; 
    qpalm_update_bounds(work, bmin, bmax);

    qpalm_warm_start(work, NULL, NULL);

    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);
    
    mu_assert_double_eq(work->solution->x[0], 0.873908, TOL);
    mu_assert_double_eq(work->solution->x[1], 0.95630465, TOL);
    mu_assert_double_eq(work->solution->x[2], 0, TOL);
    mu_assert_double_eq(work->solution->x[3], 0, TOL);
    mu_assert_double_eq(work->solution->x[4], 0, TOL);

    mu_assert_double_eq(work->solution->y[0], 0, TOL);
    mu_assert_double_eq(work->solution->y[1], 0, TOL);
    mu_assert_double_eq(work->solution->y[2], -0.339076, TOL);
    mu_assert_double_eq(work->solution->y[3], -10.0873907, TOL);
    mu_assert_double_eq(work->solution->y[4], -0.252185, TOL);
    mu_assert_double_eq(work->solution->y[5], 0, TOL);
    mu_assert_double_eq(work->solution->y[6], 2.52184767, TOL);
}
