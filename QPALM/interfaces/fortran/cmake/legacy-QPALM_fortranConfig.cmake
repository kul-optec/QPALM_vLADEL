
# Determine whether to add QUIET
set(_find_qpalm_fortran_args)
if(QPALM_fortran_FIND_QUIETLY)
    list(APPEND _find_qpalm_fortran_args QUIET)
endif()

# Determine whether to use REQUIRED COMPONENTS or OPTIONAL_COMPONENTS
if(QPALM_fortran_FIND_REQUIRED)
    list(APPEND _find_qpalm_fortran_args REQUIRED COMPONENTS fortran)
else()
    list(APPEND _find_qpalm_fortran_args OPTIONAL_COMPONENTS fortran)
endif()

string(JOIN " " _find_qpalm_fortran_args_str ${_find_qpalm_fortran_args})
message(WARNING "The QPALM_fortran package is deprecated.
Use find_package(QPALM ${_find_qpalm_fortran_args_str}) instead.
")
    
# Forward the call to QPALM
find_package(QPALM ${_find_qpalm_fortran_args})
