cmake_minimum_required(VERSION 3.20)
project(QPALM VERSION 0.0.0)
set(PY_VERSION_SUFFIX "a0")

# Options
option(QPALM_WARNINGS_AS_ERRORS "Treat warnings as errors" Off)
option(QPALM_NONCONVEX "Compile also for nonconvex QPs" On)
option(QPALM_TIMING "Enable timing" On)
option(QPALM_PRINTING "Allow printing" ON)
option(QPALM_CXX "Build the C++ interface" On)
option(QPALM_PYTHON "Build the Python interface" Off)

include(cmake/GlobalCompileOptions.cmake)

# LADEL linear algebra C library
function(add_ladel)
    set(CMAKE_POSITION_INDEPENDENT_CODE On) # scoped
    add_subdirectory(../LADEL LADEL EXCLUDE_FROM_ALL)
endfunction()
add_ladel()

# QPALM C library
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

target_compile_definitions(qpalm PUBLIC USE_LADEL=1)
if (QPALM_NONCONVEX)
    target_compile_definitions(qpalm PUBLIC COMPILE_NONCONVEX=1)
endif()
if (QPALM_TIMING)
    target_compile_definitions(qpalm PUBLIC PROFILING=1)
endif()
if (QPALM_PRINTING)
    target_compile_definitions(qpalm PUBLIC PRINTING=1)
endif()

# C++ interface
if (QPALM_CXX)
    add_subdirectory(cxx)
endif()

# Python interface
if (QPALM_PYTHON)
    add_subdirectory(python)
endif()