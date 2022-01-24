cmake_minimum_required(VERSION 3.20)
project(QPALM-dev)

set(QPALM_WARNINGS_AS_ERRORS On)

add_subdirectory(QPALM)
add_subdirectory(examples)
add_subdirectory(test)