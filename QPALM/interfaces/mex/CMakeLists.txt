cmake_minimum_required(VERSION 3.20)
project(QPALM-mex)

# Find the Matlab MEX compiler:
find_package(Matlab REQUIRED COMPONENTS MEX_COMPILER)

# Create a target for compiling a MEX function:
matlab_add_mex(
    NAME qpalm_mex                       # Name for use in Matlab
    SRC src/qpalm_mex.c                  # Source files to compile
    LINK_TO QPALM::qpalm LADEL::mex_util # Dependencies
)
target_include_directories(qpalm_mex PRIVATE include)
if (NOT WIN32 AND NOT APPLE)
    # Matlab's C++ standard library is too old, so link it statically:
    target_link_options(qpalm_mex PUBLIC -static-libstdc++ -static-libgcc)
endif()

# Install the final MEX file
install(FILES $<TARGET_FILE:qpalm_mex>
        qpalm.m
        EXCLUDE_FROM_ALL
        COMPONENT mex_interface
        DESTINATION +qpalm)