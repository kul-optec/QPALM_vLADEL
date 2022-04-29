# C++ interface
if (QPALM_WITH_CXX)
    add_subdirectory(cxx)
endif()

# Python interface
if (QPALM_WITH_PYTHON)
    add_subdirectory(python)
endif()

# Julia interface
if (QPALM_WITH_JULIA)
    add_subdirectory(julia)
endif()

# MATLAB interface
if (QPALM_WITH_MEX)
    add_subdirectory(mex)
endif()

# Fortran interface
if (QPALM_WITH_FORTRAN)
    add_subdirectory(fortran)
endif()

# Utility for solving problems from .mtx files
if (QPALM_WITH_MTX)
    add_subdirectory(mtx)
endif()

# Utility for solving problems from .qps files
if (QPALM_WITH_QPS)
    add_subdirectory(qps)
endif()

