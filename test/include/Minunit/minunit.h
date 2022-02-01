// This is a simple wrapper to easily convert the existing tests from Minunit
// to the Google Test framework.

#include <gtest/gtest.h>
#undef FAIL // defined by LADEL

#define mu_assert_long_eq EXPECT_EQ
#define mu_assert_double_eq EXPECT_NEAR
#define mu_assert_true EXPECT_TRUE
#define mu_assert_false EXPECT_FALSE
#define mu_assert_string_eq EXPECT_STREQ