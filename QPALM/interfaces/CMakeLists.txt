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
