cmake_minimum_required(VERSION 3.17)
project(QPALM-examples C CXX)

if (NOT TARGET QPALM::qpalm)
    find_package(QPALM REQUIRED OPTIONAL_COMPONENTS cxx fortran)
endif()

add_subdirectory(c)

if (TARGET QPALM::qpalm_cxx)
    add_subdirectory(cxx)
endif()

if (TARGET QPALM::qpalm_fortran)
    add_subdirectory(fortran)
endif()
