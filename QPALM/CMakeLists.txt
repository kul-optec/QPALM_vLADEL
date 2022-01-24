cmake_minimum_required(VERSION 3.20)
project(QPALM)

option(QPALM_WARNINGS_AS_ERRORS "Treat warnings as errors" Off)

include(cmake/GlobalCompileOptions.cmake)

add_subdirectory(../LADEL LADEL EXCLUDE_FROM_ALL)

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
target_link_libraries(qpalm PRIVATE qpalm_warnings)
target_link_libraries(qpalm PUBLIC ladel)
target_link_libraries(qpalm PRIVATE m)
