#include "minunit.h"
#include "qpalm.h"

#define N 15
#define M 15
#define ANZMAX 25
#define QNZMAX 15

static QPALMWorkspace *work; // Workspace
static QPALMSettings *settings;
static QPALMData *data;
static solver_common *c;
static solver_common common;
static c_float solution[N] = {-4.258643191312046e+00, 9.393193922630394e+00, 1.888905966442421e+01,
                             -2.469934088388301e+00, 9.628197800226003e+00, 6.034505999261726e+00,
                             -8.288652177085156e+00, -9.172613482098816e+00, -4.005465476438092e+01,
                             -2.983244126863757e+01, -7.447972191390734e+00, -6.315368738609618e+00,
                             4.555205430378418e+00, 6.362674847968517e+00, -2.000000000000000e+00};

void medium_qp_suite_setup(void) {
    settings = (QPALMSettings *)qpalm_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    settings->eps_abs = 1e-6;
    settings->eps_rel = 1e-6;

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
    Ap[0] = 0; Ap[1] = 1; Ap[2] = 2; Ap[3] = 5; Ap[4] = 8; Ap[5] = 9; Ap[6] = 11; 
    Ap[7] = 12; Ap[8] = 13; Ap[9] = 16; Ap[10] = 18; Ap[11] = 21; Ap[12] = 22; Ap[13] = 23; 
    Ap[14] = 24; Ap[15] = 25;
    Ai[0] = 8; Ai[1] = 2; Ai[2] = 1; Ai[3] = 4; Ai[4] = 14;
    Ai[5] = 1; Ai[6] = 4; Ai[7] = 13; Ai[8] = 5; Ai[9] = 0;
    Ai[10] = 7; Ai[11] = 10; Ai[12] = 6; Ai[13] = 1; Ai[14] = 4;
    Ai[15] = 14; Ai[16] = 0; Ai[17] = 7; Ai[18] = 1; Ai[19] = 4;
    Ai[20] = 13; Ai[21] = 3; Ai[22] = 9; Ai[23] = 11; Ai[24] = 12;
    Ax[0] = 3.256021467039615e-01; Ax[1] = -2.129201224283822e-01; Ax[2] = -3.904780212604003e-02; 
    Ax[3] = -1.097664622926547e-02; Ax[4] = 8.935098531570440e-05; Ax[5] = 1.107958814061373e-01; 
    Ax[6] = -3.941400281255630e-01; Ax[7] = -3.422661790473164e-02; Ax[8] = -2.077231940491557e-01; 
    Ax[9] = 2.961057917719591e-01; Ax[10] = 2.901671645955232e-02; Ax[11] = -2.412937540712519e-01; 
    Ax[12] = 2.180403659113273e-01; Ax[13] = -7.769757105018442e-02; Ax[14] = -2.184140217516474e-02;
    Ax[15] = -4.490435862043659e-05; Ax[16] = -7.144833411941969e-03; Ax[17] = 7.291061197330474e-02; 
    Ax[18] = 1.354927131911815e-02; Ax[19] = -4.819953694147238e-02; Ax[20] = 2.798798702152373e-01; 
    Ax[21] = -3.166877632612020e-01; Ax[22] = 4.390581348235377e-01; Ax[23] = -3.143332085622074e-01; 
    Ax[24] = -1.000000000000000e+00;  

    c_float *Qx = data->Q->x;
    c_int *Qi = data->Q->i;
    c_int *Qp = data->Q->p;
    Qp[0] = 0; Qp[1] = 1; Qp[2] = 2; Qp[3] = 3; Qp[4] = 4; Qp[5] = 5; Qp[6] = 6; 
    Qp[7] = 7; Qp[8] = 8; Qp[9] = 9; Qp[10] = 10; Qp[11] = 11; Qp[12] = 12; Qp[13] = 13; 
    Qp[14] = 14; Qp[15] = 15;
    Qi[0] = 0; Qi[1] = 1; Qi[2] = 2; Qi[3] = 3; Qi[4] = 4; Qi[5] = 5; Qi[6] = 6; 
    Qi[7] = 7; Qi[8] = 8; Qi[9] = 9; Qi[10] = 10; Qi[11] = 11; Qi[12] = 12; Qi[13] = 13; 
    Qi[14] = 14; 
    Qx[0] = 1.000000000000000e+00; Qx[1] = 5.179474679231212e-01; Qx[2] = 2.682695795279726e-01; 
    Qx[3] = 1.389495494373138e-01; Qx[4] = 7.196856730011525e-02; Qx[5] = 3.727593720314943e-02; 
    Qx[6] = 1.930697728883252e-02; Qx[7] = 1.000000000000001e-02; Qx[8] = 5.179474679231217e-03; 
    Qx[9] = 2.682695795279729e-03; Qx[10] = 1.389495494373140e-03; Qx[11] = 7.196856730011531e-04; 
    Qx[12] = 3.727593720314947e-04; Qx[13] = 1.930697728883254e-04; Qx[14] = 1.000000000000002e-04;

    data->q = (c_float *)qpalm_calloc(N,sizeof(c_float));
    data->bmin = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmax = (c_float *)qpalm_calloc(M,sizeof(c_float));
    data->bmin[0] = -2; 
    data->bmin[1] = -2;
    data->bmin[2] = -2;
    data->bmin[3] = -2;
    data->bmin[4] = -2;
    data->bmin[5] = -2;
    data->bmin[6] = -2;
    data->bmin[7] = -2;
    data->bmin[8] = -2;
    data->bmin[9] = -2;
    data->bmin[10] = -2;
    data->bmin[11] = -2;
    data->bmin[12] = -2;
    data->bmin[13] = -2;
    data->bmin[14] = -2;

    data->bmax[0] = 2; 
    data->bmax[1] = 2;
    data->bmax[2] = 2;
    data->bmax[3] = 2;
    data->bmax[4] = 2;
    data->bmax[5] = 2;
    data->bmax[6] = 2;
    data->bmax[7] = 2;
    data->bmax[8] = 2;
    data->bmax[9] = 2;
    data->bmax[10] = 2;
    data->bmax[11] = 2;
    data->bmax[12] = 2;
    data->bmax[13] = 2;
    data->bmax[14] = 2;

    data->q[0] = 4.258643191312094e+00; 
    data->q[1] = -1.270043450597050e+01; 
    data->q[2] = -4.852188357430427e+00; 
    data->q[3] = 5.943076168298481e+00;
    data->q[4] = -2.764649066392558e+00; 
    data->q[5] = -1.857582885927374e+01; 
    data->q[6] = 4.073081174942876e-01; 
    data->q[7] = 2.829701771619900e+00;
    data->q[8] = 6.356121930249937e-01;
    data->q[9] = 4.334300651115951e+00;
    data->q[10] = 4.228603644876851e+00;
    data->q[11] = 1.299528296551999e+01;
    data->q[12] = -1.049793234475067e+01;
    data->q[13] = -1.786411722110915e+01;
    data->q[14] = 8.160430810319180e+00;     
}

void medium_qp_suite_teardown(void) {
    qpalm_free(settings);
    // Clean setup
    data->Q = ladel_sparse_free(data->Q);
    data->A = ladel_sparse_free(data->A);
    qpalm_free(data->q);
    qpalm_free(data->bmin);
    qpalm_free(data->bmax);
    qpalm_free(data);
}

void medium_qp_test_teardown(void) {
    qpalm_cleanup(work);
}

struct TestMediumQP : ::testing::TestWithParam<int> {
    void SetUp() override {
        medium_qp_suite_setup();
        // medium_qp_test_setup();
        settings->max_rank_update_fraction = 1.0;
        settings->factorization_method = GetParam();
    }
    void TearDown() override {
        medium_qp_test_teardown();
        medium_qp_suite_teardown();
    }
};

TEST_P(TestMediumQP, test_medium_qp) {
    // Setup workspace
    work = qpalm_setup(data, settings);
    // Solve Problem
    qpalm_solve(work);

    mu_assert_long_eq(work->info->status_val, QPALM_SOLVED);
    c_float tol;
    for(c_int i = 0; i < N; i++) {
        tol = c_absval(1e-5*solution[i]);
        mu_assert_double_eq(work->solution->x[i], solution[i], tol);
    }
}

INSTANTIATE_TEST_SUITE_P(TestMediumQP, TestMediumQP,
                         ::testing::Values(FACTORIZE_KKT_OR_SCHUR, 
                                           FACTORIZE_KKT,
                                           FACTORIZE_SCHUR));
