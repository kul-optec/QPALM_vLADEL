enable_language(Fortran)
add_executable(qpalm_demo_fortran "qpalm_fortran_demo.F90")
target_link_libraries(qpalm_demo_fortran PRIVATE QPALM::qpalm_fortran)