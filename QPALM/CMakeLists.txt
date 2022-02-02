cmake_minimum_required(VERSION 3.20)
project(QPALM VERSION 0.0.0)
set(PY_VERSION_SUFFIX "a1")

# Options
option(QPALM_WARNINGS_AS_ERRORS "Treat warnings as errors" Off)
option(QPALM_NONCONVEX "Compile also for nonconvex QPs" On)
option(QPALM_TIMING "Enable timing" On)
option(QPALM_PRINTING "Allow printing" On)
option(QPALM_WITH_CXX "Build the C++ interface" Off)
option(QPALM_WITH_PYTHON "Build the Python interface" Off)
option(QPALM_WITH_MEX "Build the MATLAB interface" Off)

include(cmake/GlobalCompileOptions.cmake)

# LADEL linear algebra C library
function(add_ladel)
    set(CMAKE_POSITION_INDEPENDENT_CODE On) # scoped
    set(LADEL_WITH_MEX ${QPALM_WITH_MEX})
    add_subdirectory(../LADEL/LADEL LADEL EXCLUDE_FROM_ALL)
endfunction()
add_ladel()

cmake_policy(SET CMP0094 NEW)
set(Python3_FIND_REGISTRY NEVER)

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
    "src/global_opts.c"
)
target_include_directories(qpalm
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
target_link_libraries(qpalm PRIVATE qpalm_lax_warnings)
target_link_libraries(qpalm PUBLIC ladel)
set_target_properties(qpalm PROPERTIES POSITION_INDEPENDENT_CODE On)
target_compile_definitions(qpalm 
    PUBLIC QPALM_VERSION_STR=\"${QPALM_VERSION}${PY_VERSION_SUFFIX}\"
           QPALM_VERSION_MAJOR=${QPALM_VERSION_MAJOR}
           QPALM_VERSION_MINOR=${QPALM_VERSION_MINOR}
           QPALM_VERSION_PATCH=${QPALM_VERSION_PATCH})
if (NOT WIN32)
    target_link_libraries(qpalm PRIVATE m)
endif()

target_compile_definitions(qpalm PUBLIC USE_LADEL)
if (QPALM_NONCONVEX)
    target_compile_definitions(qpalm PUBLIC COMPILE_NONCONVEX)
endif()
if (QPALM_TIMING)
    target_compile_definitions(qpalm PUBLIC PROFILING)
endif()
if (QPALM_PRINTING)
    target_compile_definitions(qpalm PUBLIC PRINTING)
endif()
if (QPALM_WITH_PYTHON)
    find_package(Python3 REQUIRED COMPONENTS Development)
    target_link_libraries(qpalm PUBLIC Python3::Module)
endif()
add_library(qpalm::qpalm ALIAS qpalm)

add_subdirectory(interfaces)