#include "minunit.h"
#include "qpalm.h"

#define N 2
#define M 2
#define ANZMAX 2
#define QNZMAX 2
#define TOL 1e-12

static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
// QPALMData *data;
static c_float *w;
static solver_common *c;
static solver_common common;

void update_after_setup_suite_setup(void) {
    settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-8;
    settings->eps_rel = 1e-8;

    c_float* dummy = (c_float *)qpalm_calloc(M, sizeof(c_float)); dummy[0] = 0; dummy[1] = 0;
    c_int *Ai = (c_int *)qpalm_calloc(ANZMAX, sizeof(c_int)); 
    c_int *Ap = (c_int *)qpalm_calloc(N+1, sizeof(c_int));
    Ap[0] = 0; Ap[1] = 1; Ap[2] = 2; 
    Ai[0] = 0; Ai[1] = 1; 

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
    Qp[0] = 0; Qp[1] = 1; Qp[2] = 2; 
    Qi[0] = 0; Qi[1] = 1; 

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

void update_after_setup_suite_teardown(void) {
    qpalm_free(settings);
    // Clean setup
    qpalm_free(w);
}

void update_after_setup_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestUpdateAfterSetup : ::testing::Test {
    void SetUp() override {
        update_after_setup_suite_setup();
        // update_after_setup_test_setup();
    }
    void TearDown() override {
        update_after_setup_test_teardown();
        update_after_setup_suite_teardown();
    }
};

TEST_F(TestUpdateAfterSetup, test_update_after_setup) {
    // Setup workspace
    // work = qpalm_setup(data, settings);
    c_float *Qx = w, *Ax = w+QNZMAX;
    Qx[0] = 1.0; Qx[1] = 1.0;
    Ax[0] = 1; Ax[1] = 1;
    qpalm_update_Q_A(work, Qx, Ax);

    c_float *q = w;
    q[0] = -0.7; q[1] = -2.3;
    qpalm_update_q(work, q);

    c_float *bmin = w, *bmax = w+M;
    bmin[0] = -QPALM_INFTY; bmin[1] = -QPALM_INFTY;
    bmax[0] = QPALM_INFTY; bmax[1] = QPALM_INFTY;
    qpalm_update_bounds(work, bmin, bmax);

    qpalm_warm_start(work, NULL, NULL);

    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);
    mu_assert_double_eq(work->solution->x[0], 0.7, TOL);
    mu_assert_double_eq(work->solution->x[1], 2.3, TOL);

    c_float x_temp[2];
    x_temp[0] = work->solution->x[0];
    x_temp[1] = work->solution->x[1];

    Qx = w; Ax = w+QNZMAX;
    Qx[0] = 1.0; Qx[1] = 1.0;
    Ax[0] = 1; Ax[1] = 1;
    qpalm_update_Q_A(work, Qx, Ax);

    q = w;
    q[0] = -0.7; q[1] = -2.3;
    qpalm_update_q(work, q);

    bmin = w; bmax = w+M;
    bmin[0] = -QPALM_INFTY; bmin[1] = -QPALM_INFTY;
    bmax[0] = QPALM_INFTY; bmax[1] = QPALM_INFTY;
    qpalm_update_bounds(work, bmin, bmax);

    qpalm_warm_start(work, NULL, NULL);

    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);
    //Should be exactly the same as before
    mu_assert_double_eq(work->solution->x[0], x_temp[0], 0); 
    mu_assert_double_eq(work->solution->x[1], x_temp[1], 0);
}
