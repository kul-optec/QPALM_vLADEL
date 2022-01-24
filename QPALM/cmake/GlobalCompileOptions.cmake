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
    -Wno-error=unused-but-set-variable
)
set(CLANG_WARNINGS
    -Wno-newline-eof
)

if (QPALM_WARNINGS_AS_ERRORS)
    list(APPEND COMMON_WARNINGS -Werror)
endif()

add_library(qpalm_warnings INTERFACE)
add_library(qpalm_lax_warnings INTERFACE)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(qpalm_warnings INTERFACE
        ${COMMON_WARNINGS})
    target_compile_options(qpalm_lax_warnings INTERFACE
        ${COMMON_WARNINGS} ${COMMON_LAX_WARNINGS})
elseif (CMAKE_C_COMPILER_ID MATCHES ".*Clang")
    target_compile_options(qpalm_warnings INTERFACE
        ${COMMON_WARNINGS} ${CLANG_WARNINGS})
    target_compile_options(qpalm_lax_warnings INTERFACE
        ${COMMON_WARNINGS} ${COMMON_LAX_WARNINGS} ${CLANG_WARNINGS})
else() # TODO: Clang, MSVC
    message(FATAL_ERROR "No known warnings for this compiler")
endif()
