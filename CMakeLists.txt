# This is the CMake file for developers. If you are a user or a packager, please
# see QPALM/CMakeLists.txt instead.

cmake_minimum_required(VERSION 3.17)
project(QPALM-dev)
enable_testing()

# Add coverage target
option(QPALM_WITH_COVERAGE
    "Generate coverage information." Off)
if (QPALM_WITH_COVERAGE)
    add_custom_target(coverage
        ${CMAKE_CURRENT_LIST_DIR}/scripts/coverage.sh
        ${CMAKE_CXX_COMPILER_ID}
        ${CMAKE_CXX_COMPILER_VERSION}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fno-inline")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fno-inline")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    add_dependencies(coverage QPALM::tests)
endif()

# Add documentation target
find_program(QPALM_DOXYGEN doxygen)
if (QPALM_DOXYGEN)
    add_custom_target(documentation
        doxygen
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/doxygen)
endif()

set(QPALM_WARNINGS_AS_ERRORS On)

# Source code
add_subdirectory(QPALM)

# Examples
option(QPALM_WITH_EXAMPLES
    "Build the example programs" On)
if (QPALM_WITH_EXAMPLES)
    add_subdirectory(examples)
endif()

# Unit tests
option(QPALM_WITH_TESTS
    "Build the unit tests" On)
if (QPALM_WITH_TESTS)
    add_subdirectory(test)
endif()