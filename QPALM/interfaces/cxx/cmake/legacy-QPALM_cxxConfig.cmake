# Determine whether to add QUIET
set(_find_qpalm_cxx_args)
if(QPALM_cxx_FIND_QUIETLY)
    list(APPEND _find_qpalm_cxx_args QUIET)
endif()

# Determine whether to use REQUIRED COMPONENTS or OPTIONAL_COMPONENTS
if(QPALM_cxx_FIND_REQUIRED)
    list(APPEND _find_qpalm_cxx_args REQUIRED COMPONENTS cxx)
else()
    list(APPEND _find_qpalm_cxx_args OPTIONAL_COMPONENTS cxx)
endif()

string(JOIN " " _find_qpalm_cxx_args_str ${_find_qpalm_cxx_args})
message(WARNING "The QPALM_cxx package is deprecated.
Use find_package(QPALM ${_find_qpalm_cxx_args_str}) instead.
")

# Forward the call to QPALM
find_package(QPALM ${_find_qpalm_cxx_args})
