#include "minunit.h"
#include "qpalm.h"

#define N 2
#define M 3
#define ANZMAX 5
#define QNZMAX 4
#define TOL 1e-8

static QPALMWorkspace *work; // Workspace
static solver_sparse *A; // MxN matrix
static solver_sparse *Q; // NxN symmetric matrix
static solver_common common, *c;


void solver_suite_setup(void) {
    QPALMSettings *settings = (QPALMSettings *)c_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-6;
    settings->eps_rel = 1e-6;

    QPALMData *data    = (QPALMData *)c_malloc(sizeof(QPALMData));
    data->n = N;
    data->m = M;
    data->c = 0;
    c_float q[N] = {1, -2};
    data->q = q;
    c_float bmin[M] = {-5, -10, -2};
    c_float bmax[M] = {5, 10, 3};
    data->bmin = bmin;
    data->bmax = bmax;

    A = ladel_sparse_alloc(M, N, ANZMAX, UNSYMMETRIC, TRUE, FALSE);
    Q = ladel_sparse_alloc(N, N, QNZMAX, UPPER, TRUE, FALSE);
    c_float *Ax;
    c_int *Ai, *Ap;
    Ax = A->x;
    Ap = A->p;
    Ai = A->i;
    Ax[0] = 1.0; Ax[1] = 3.0; Ax[2] = 5.0; Ax[3] = 2.0; Ax[4] = 4.0;  
    Ap[0] = 0; Ap[1] = 3; Ap[2] = 5;
    Ai[0] = 0; Ai[1] = 1; Ai[2] = 2; Ai[3] = 0; Ai[4] = 1;
 
    c_float *Qx;
    c_int *Qi, *Qp;
    Qx = Q->x;
    Qp = Q->p;
    Qi = Q->i;
    Qx[0] = 1.0; Qx[1] = -1.0; Qx[2] = -1.0; Qx[3] = 2.0; 
    Qp[0] = 0; Qp[1] = 2; Qp[2] = 4; 
    Qi[0] = 0; Qi[1] = 1; Qi[2] = 0; Qi[3] = 1; 

    ladel_to_upper_diag(Q);

    data->A = A;
    data->Q = Q;
    
    // Setup workspace
    work = qpalm_setup(data, settings);

    c_free(data);
    c_free(settings);
}

void solver_suite_teardown(void) {
    qpalm_cleanup(work);

    Q = ladel_sparse_free(Q);
    A = ladel_sparse_free(A);

}

void solver_test_setup(void) {
    work->Qd[0] = 1.1; work->Qd[1] = -0.5;
    work->Ad[0] = 1.1; work->Ad[1] = -0.5; work->Ad[2] = 20;
}

void solver_test_teardown(void) {
}

struct TestSolver : ::testing::Test {
    void SetUp() override {
        solver_suite_setup();
        solver_test_setup();
    }
    void TearDown() override {
        solver_test_teardown();
        solver_suite_teardown();
    }
};

TEST_F(TestSolver, test_mat_vec){
    mat_vec(A, work->solver->Qd, work->solver->Ad, c);
    mu_assert_double_eq(work->Ad[0], 0.1, TOL);
    mu_assert_double_eq(work->Ad[1], 1.3, TOL);
    mu_assert_double_eq(work->Ad[2], 5.5, TOL);

    mat_vec(Q, work->solver->Qd, work->solver->Qd, c);
    mu_assert_double_eq(work->Qd[0], 1.6, TOL);
    mu_assert_double_eq(work->Qd[1], -2.1, TOL);
    work->Qd[0] = 1.1; work->Qd[1] = -0.5;

    mat_tpose_vec(Q, work->solver->Qd, work->solver->Qd, c);
    mu_assert_double_eq(work->Qd[0], 1.6, TOL);
    mu_assert_double_eq(work->Qd[1], -2.1, TOL);

}

TEST_F(TestSolver, test_mat_tpose_vec){
    mat_tpose_vec(A, work->solver->Ad, work->solver->Qd, c);
    mu_assert_double_eq(work->Qd[0], 99.6, TOL);
    mu_assert_double_eq(work->Qd[1], 0.2, TOL);
}

TEST_F(TestSolver, test_mat_inf_norm_cols){
    mat_inf_norm_cols(A, work->D_temp);
    mu_assert_double_eq(work->D_temp[0], 5.0, TOL);
    mu_assert_double_eq(work->D_temp[1], 4.0, TOL);
}

TEST_F(TestSolver, test_mat_inf_norm_rows){
    mat_inf_norm_rows(A, work->E_temp);
    mu_assert_double_eq(work->E_temp[0], 2.0, TOL);
    mu_assert_double_eq(work->E_temp[1], 4.0, TOL);
    mu_assert_double_eq(work->E_temp[2], 5.0, TOL);
}
