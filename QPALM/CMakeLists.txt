cmake_minimum_required(VERSION 3.20)
project(QPALM VERSION 0.0.0)
set(PY_VERSION_SUFFIX "a0")

option(QPALM_WARNINGS_AS_ERRORS "Treat warnings as errors" Off)

include(cmake/GlobalCompileOptions.cmake)

function(add_ladel)
    set(CMAKE_POSITION_INDEPENDENT_CODE On) # scoped
    add_subdirectory(../LADEL LADEL EXCLUDE_FROM_ALL)
endfunction()
add_ladel()

add_library(qpalm 
    "src/iteration.c"
    "src/lin_alg.c"
    "src/linesearch.c"
    "src/newton.c"
    "src/nonconvex.c"
    "src/qpalm.c"
    "src/scaling.c"
    "src/solver_interface.c"
    "src/termination.c"
    "src/util.c"
    "src/validate.c"
)
target_include_directories(qpalm
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
target_link_libraries(qpalm PRIVATE qpalm_lax_warnings)
target_link_libraries(qpalm PUBLIC ladel)
target_link_libraries(qpalm PRIVATE m)
set_target_properties(qpalm PROPERTIES POSITION_INDEPENDENT_CODE On)

find_package(Eigen3 REQUIRED)

add_library(qpalm_cxx
    "src/cxx/sparse.cpp"
    "src/cxx/qpalm.cpp"
    "include/cxx/sparse.hpp"
    "include/cxx/qpalm.hpp"
)
target_link_libraries(qpalm_cxx PRIVATE qpalm_warnings)
target_link_libraries(qpalm_cxx PUBLIC qpalm Eigen3::Eigen)
set_target_properties(qpalm_cxx PROPERTIES POSITION_INDEPENDENT_CODE On)

add_subdirectory(python)