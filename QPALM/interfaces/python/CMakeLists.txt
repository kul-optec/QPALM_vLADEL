cmake_minimum_required(VERSION 3.20)
set(PY_FULL_VERSION ${PROJECT_VERSION}${PY_VERSION_SUFFIX})
set(PY_BUILD_CMAKE_MODULE_NAME "qpalm" CACHE STRING "Python module name")
set(PY_BUILD_CMAKE_PACKAGE_NAME "qpalm" CACHE STRING "Python package name")

# Make sure that the Python and CMake versions match
if (DEFINED VERIFY_VERSION)
    if (NOT "${VERIFY_VERSION}" MATCHES "^${PY_FULL_VERSION}$")
        message(FATAL_ERROR "Version number does not match "
                             "(${VERIFY_VERSION} - ${PY_FULL_VERSION}).")
    endif()
endif()

# Python interface requires the C++ interface
if (NOT QPALM_WITH_CXX)
    message(FATAL_ERROR "QPALM Python interface requires the C++ interface."
                        "Enable it using -DQPALM_WITH_CXX=On.")
endif()

# Find the Pybind11 headers
include(cmake/QueryPythonForPybind11.cmake)
find_pybind11_python_first()

# Compile the example Python module
pybind11_add_module(_qpalm MODULE "qpalm.py.cpp")
target_compile_features(_qpalm PRIVATE cxx_std_17)
target_link_libraries(_qpalm PRIVATE qpalm_warnings)
target_link_libraries(_qpalm PRIVATE pybind11::pybind11 qpalm qpalm_cxx)
target_compile_definitions(_qpalm PRIVATE
    MODULE_NAME=$<TARGET_FILE_BASE_NAME:_qpalm>
    VERSION_INFO="${PY_FULL_VERSION}")
set_target_properties(_qpalm PROPERTIES 
    CXX_VISIBILITY_PRESET hidden
    C_VISIBILITY_PRESET hidden)
if (NOT WIN32 AND NOT APPLE)
    target_link_options(_qpalm PRIVATE "LINKER:--exclude-libs,ALL")
endif()

# Install the Python module
install(TARGETS _qpalm
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME})

# Strip and install debug information
set(DEBUG_FILE "$<TARGET_FILE_NAME:_qpalm>.debug")
if (CMAKE_STRIP AND CMAKE_OBJCOPY)
    add_custom_command(TARGET _qpalm POST_BUILD
        COMMAND "${CMAKE_STRIP}" "--only-keep-debug" "$<TARGET_FILE:_qpalm>" "-o" "${DEBUG_FILE}"
        COMMAND "${CMAKE_STRIP}" "--strip-debug" "$<TARGET_FILE:_qpalm>"
        COMMAND "${CMAKE_OBJCOPY}" "--add-gnu-debuglink=${DEBUG_FILE}" "$<TARGET_FILE:_qpalm>"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DEBUG_FILE}"
        EXCLUDE_FROM_ALL
        CONFIGURATIONS Debug RelWithDebInfo
        COMPONENT python_modules_debug
        DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME})
endif()

# Generate stubs for the Python module
option(QPALM_WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module." On)
if (QPALM_WITH_PY_STUBS AND NOT CMAKE_CROSSCOMPILING)
    include(cmake/Pybind11Stubgen.cmake)
    pybind11_stubgen(_qpalm)
    pybind11_stubgen_install(_qpalm ${PY_BUILD_CMAKE_MODULE_NAME})
endif()
