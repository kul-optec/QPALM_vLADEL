set(PY_FULL_VERSION ${PROJECT_VERSION}${PY_VERSION_SUFFIX})

# Make sure that the Python and CMake versions match
if (DEFINED VERIFY_VERSION)
    if (NOT "${VERIFY_VERSION}" MATCHES "^${PY_FULL_VERSION}$")
        message(FATAL_ERROR "Version number does not match "
                             "(${VERIFY_VERSION} - ${PY_FULL_VERSION}).")
    endif()
endif()

# Find the Pybind11 headers
include(cmake/QueryPythonForPybind11.cmake)
find_pybind11_python_first()

# Compile the example Python module
Python3_add_library(_qpalm MODULE "qpalm.cpp")
target_compile_features(_qpalm PRIVATE cxx_std_17)
target_link_libraries(_qpalm PRIVATE qpalm_warnings)
target_link_libraries(_qpalm PRIVATE pybind11::pybind11 qpalm qpalm_cxx)
target_compile_definitions(_qpalm PRIVATE
    MODULE_NAME=_qpalm
    VERSION_INFO="${PY_FULL_VERSION}"
)
if (NOT WIN32)
    target_link_options(_qpalm PRIVATE "LINKER:--exclude-libs,ALL")
endif()

# Install the Python module
install(TARGETS _qpalm
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION qpalm)

# Generate stubs for the Python module
option(QPALM_WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module." On)
if (QPALM_WITH_PY_STUBS)
    include(cmake/Pybind11Stubgen.cmake)
    pybind11_stubgen(_qpalm)
    pybind11_stubgen_install(_qpalm qpalm)
endif()
