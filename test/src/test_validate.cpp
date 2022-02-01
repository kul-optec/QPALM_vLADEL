#include "minunit.h"
#include "qpalm.h"
#include <stdio.h>

#define M 3

static QPALMSettings *settings;
static QPALMData *data;

void validate_suite_setup(void) {

    settings = (QPALMSettings *)c_malloc(sizeof(QPALMSettings));
    qpalm_set_default_settings(settings);
    data = (QPALMData *)c_malloc(sizeof(QPALMData));
    data->m = M;
    data->c = 0;

    data->bmin = (c_float *)c_calloc(M,sizeof(c_float));
    data->bmin[0] = -1.0; data->bmin[1] = -1.0; data->bmin[2] = -1.0; 
    data->bmax = (c_float *)c_calloc(M,sizeof(c_float));
    data->bmax[0] = 1.0; data->bmax[1] = 1.0; data->bmax[2] = 1.0; 
}

void validate_suite_teardown(void) {
    c_free(settings);
    if (data) {
        c_free(data->bmin);
        c_free(data->bmax);
        c_free(data);
    }
}

void validate_test_setup(void) {
    if (data==NULL) {
        data = (QPALMData *)c_malloc(sizeof(QPALMData));
        data->m = M;
        data->bmin = (c_float *)c_calloc(M,sizeof(c_float));
        data->bmax = (c_float *)c_calloc(M,sizeof(c_float));
    }
    if (settings == NULL) {
        settings = (QPALMSettings *)c_malloc(sizeof(QPALMSettings));
    }
    
    qpalm_set_default_settings(settings);

    data->bmin[0] = -1.0; data->bmin[1] = -1.0; data->bmin[2] = -1.0;
    data->bmax[0] = 1.0; data->bmax[1] = 1.0; data->bmax[2] = 1.0;
}

struct TestValidate : ::testing::Test {
    void SetUp() override {
        validate_suite_setup();
        validate_test_setup();
    }
    void TearDown() override {
        // validate_test_teardown();
        validate_suite_teardown();
    }
};

/* Data validation */
TEST_F(TestValidate, test_correct_data) {
    mu_assert_true(validate_data(data));
}

TEST_F(TestValidate, test_missing_data) {
    c_free(data->bmin);
    c_free(data->bmax);
    c_free(data);
    data=NULL;
    mu_assert_false(validate_data(data));
}

TEST_F(TestValidate, test_bounds_mismatch) {
    data->bmin[1] = 2; //higher than the upper bound
    mu_assert_false(validate_data(data));
}

/*Settings validation */
TEST_F(TestValidate, test_correct_settings) {
    mu_assert_true(validate_settings(settings));
}

TEST_F(TestValidate, test_missing_settings) {
    c_free(settings);
    settings=NULL;
    mu_assert_false(validate_settings(settings));
}

TEST_F(TestValidate, test_iter_out_of_bounds){
    settings->max_iter = -1;
    mu_assert_false(validate_settings(settings));
    settings->max_iter = 0;
    mu_assert_false(validate_settings(settings));
    settings->max_iter = 1;
    mu_assert_true(validate_settings(settings));

    settings->inner_max_iter = -1;
    mu_assert_false(validate_settings(settings));
    settings->inner_max_iter = 0;
    mu_assert_false(validate_settings(settings));
    settings->inner_max_iter = 1;
    mu_assert_true(validate_settings(settings));

    settings->print_iter = -1;
    mu_assert_false(validate_settings(settings));
    settings->print_iter = 0;
    mu_assert_false(validate_settings(settings));
    settings->print_iter = 1;
    mu_assert_true(validate_settings(settings));

    settings->reset_newton_iter = -1;
    mu_assert_false(validate_settings(settings));
    settings->reset_newton_iter = 0;
    mu_assert_false(validate_settings(settings));
    settings->reset_newton_iter = 1;
    mu_assert_true(validate_settings(settings));
}

TEST_F(TestValidate, test_tol_out_of_bounds){
    settings->eps_abs = -1;
    mu_assert_false(validate_settings(settings));
    settings->eps_abs = 1;

    settings->eps_rel = -1;
    mu_assert_false(validate_settings(settings));

    settings->eps_rel = 0; settings->eps_abs = 0;
    mu_assert_false(validate_settings(settings));
    settings->eps_abs = 1;

    settings->eps_abs_in = -1;
    mu_assert_false(validate_settings(settings));
    settings->eps_abs_in = 1;

    settings->eps_rel_in = -1;
    mu_assert_false(validate_settings(settings));
    settings->eps_rel_in = 0; settings->eps_abs_in = 0;
    mu_assert_false(validate_settings(settings));
    settings->eps_abs_in = 1;

    settings->eps_dual_inf = -1;
    mu_assert_false(validate_settings(settings));
    settings->eps_dual_inf = 1;

    settings->eps_prim_inf = -1;
    mu_assert_false(validate_settings(settings));
}

TEST_F(TestValidate, test_rho_out_of_bounds){
    settings->rho = 0;
    mu_assert_false(validate_settings(settings));
    settings->rho = 1;
    mu_assert_false(validate_settings(settings));
    settings->rho = 0.5;
    mu_assert_true(validate_settings(settings));
}

TEST_F(TestValidate, test_theta_out_of_bounds){
    settings->theta = 1;
    mu_assert_true(validate_settings(settings));
    settings->theta = 2;
    mu_assert_false(validate_settings(settings));
}

TEST_F(TestValidate, test_delta_out_of_bounds){
    settings->delta = 0.5;
    mu_assert_false(validate_settings(settings));
    settings->delta = 1;
    mu_assert_false(validate_settings(settings));
    settings->delta = 1.1;
    mu_assert_true(validate_settings(settings));
}

TEST_F(TestValidate, test_sigma_max_out_of_bounds){
    settings->sigma_max = -0.5;
    mu_assert_false(validate_settings(settings));
    settings->sigma_max = 0.0;
    mu_assert_false(validate_settings(settings));
    settings->sigma_max = 0.5;
    mu_assert_true(validate_settings(settings));
}

TEST_F(TestValidate, test_gamma_out_of_bounds){
    settings->gamma_init = 0;
    mu_assert_false(validate_settings(settings));
    settings->gamma_init = 1e5;
    
    settings->gamma_upd = 0.5;
    mu_assert_false(validate_settings(settings));
    settings->gamma_upd = 1;
    mu_assert_true(validate_settings(settings));

    settings->gamma_max = settings->gamma_init;
    mu_assert_true(validate_settings(settings));
    settings->gamma_max /= 10;
    mu_assert_false(validate_settings(settings));
}

TEST_F(TestValidate, test_scaling_out_of_bounds){
    settings->scaling = -1;
    mu_assert_false(validate_settings(settings));
    settings->scaling = 0;
    mu_assert_true(validate_settings(settings));
}

TEST_F(TestValidate, test_booleans){
    settings->proximal = FALSE;
    mu_assert_true(validate_settings(settings));
    settings->proximal = 3;
    mu_assert_false(validate_settings(settings));
    settings->proximal = TRUE;

    settings->warm_start = FALSE;
    mu_assert_true(validate_settings(settings));
    settings->warm_start = 3;
    mu_assert_false(validate_settings(settings));
    settings->warm_start = TRUE;

    settings->verbose = FALSE;
    mu_assert_true(validate_settings(settings));
    settings->verbose = 3;
    mu_assert_false(validate_settings(settings));
    settings->verbose = TRUE;

    settings->enable_dual_termination = FALSE;
    mu_assert_true(validate_settings(settings));
    settings->enable_dual_termination = 3;
    mu_assert_false(validate_settings(settings));
    settings->enable_dual_termination = TRUE;
}
