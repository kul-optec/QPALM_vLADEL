include(GoogleTest)
find_package(GTest REQUIRED CONFIG)

add_executable(tests
    "src/test_ls_qp.cpp"
    "src/test_lin_alg.cpp"
    "src/test_update_after_setup.cpp"
    "src/test_medium_qp.cpp"
    "src/test_update.cpp"
    "src/test_casadi_general_unconstrained.cpp"
    "src/test_dua_inf_qp.cpp"
    "src/test_validate.cpp"
    "src/test_error_handling.cpp"
    "src/test_nonconvex_qp.cpp"
    "src/test_prim_inf_qp.cpp"
    "src/test_solver_interface.cpp"
    "src/test_casadi_general_convex_sparse.cpp"
    "src/test_basic_qp.cpp"
    "src/test_degen_hess.cpp"
)
target_include_directories(tests
    PRIVATE include
            include/Minunit
)

target_link_libraries(tests PRIVATE QPALM::qpalm)
target_link_libraries(tests PRIVATE GTest::gtest_main)

gtest_discover_tests(tests)
add_executable(qpalm::tests ALIAS tests)
