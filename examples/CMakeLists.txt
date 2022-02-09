cmake_minimum_required(VERSION 3.17)
project(QPALM-examples C CXX)

if (NOT TARGET QPALM::qpalm)
    find_package(QPALM REQUIRED)
endif()
if (NOT TARGET QPALM::qpalm_cxx)
    find_package(QPALM_cxx QUIET)
endif()

add_subdirectory(c)

if (TARGET QPALM::qpalm_cxx)
    add_subdirectory(cxx)
endif()
