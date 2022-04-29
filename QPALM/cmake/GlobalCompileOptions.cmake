set(COMMON_WARNINGS 
    -Wall
    -Wextra
    -Wpedantic
    # TODO: this isn't enough
    # -fanalyzer
)
set(COMMON_LAX_WARNINGS
    -Wno-error=unused-parameter
    -Wno-error=unused-variable
    -Wno-error=format
    -Wno-error=pedantic
)
set(GCC_WARNINGS
    -Wno-error=unused-but-set-variable
)
set(CLANG_WARNINGS
    -Wno-error=unknown-warning-option
    -Wno-newline-eof
    -Wno-error=unused-but-set-variable
)
set(MSVC_WARNINGS
    /W4
    /wd4127 # conditional expression is constant
    /wd4458 # declaration of 'x' hides class member
    /permissive-
)
set(MSVC_LAX_WARNINGS ${MSVC_WARNINGS})

set(INTEL_WARNINGS 
    -Wall 
    -Wextra
)
set(INTEL_LAX_WARNINGS ${INTEL_WARNINGS})

if (QPALM_WARNINGS_AS_ERRORS)
    if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
        list(APPEND MSVC_WARNINGS /WX)
    else()
        list(APPEND COMMON_WARNINGS -Werror)
        list(APPEND COMMON_LAX_WARNINGS -Werror)
    endif()
endif()

add_library(qpalm_warnings INTERFACE)
add_library(qpalm_lax_warnings INTERFACE)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(qpalm_warnings INTERFACE
        ${COMMON_WARNINGS})
    target_compile_options(qpalm_lax_warnings INTERFACE
        ${COMMON_WARNINGS} ${COMMON_LAX_WARNINGS} ${GCC_WARNINGS})
elseif (CMAKE_C_COMPILER_ID MATCHES ".*Clang")
    target_compile_options(qpalm_warnings INTERFACE
        ${COMMON_WARNINGS} ${CLANG_WARNINGS})
    target_compile_options(qpalm_lax_warnings INTERFACE
        ${COMMON_WARNINGS} ${COMMON_LAX_WARNINGS} ${CLANG_WARNINGS})
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
    target_compile_options(qpalm_warnings INTERFACE
        ${MSVC_WARNINGS})
    target_compile_options(qpalm_lax_warnings INTERFACE
        ${MSVC_LAX_WARNINGS})
elseif (CMAKE_C_COMPILER_ID MATCHES "Intel")
    target_compile_options(qpalm_warnings INTERFACE
        ${INTEL_WARNINGS})
    target_compile_options(qpalm_lax_warnings INTERFACE
        ${INTEL_LAX_WARNINGS})
else()
    message(WARNING "No known warnings for this compiler")
endif()
add_library(${PROJECT_NAME}::qpalm_lax_warnings ALIAS qpalm_lax_warnings)
add_library(${PROJECT_NAME}::qpalm_warnings ALIAS qpalm_warnings)

set(CMAKE_DEBUG_POSTFIX "d")
