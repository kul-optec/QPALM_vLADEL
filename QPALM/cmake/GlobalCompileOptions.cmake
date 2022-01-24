set(COMMON_WARNINGS 
    -Wall
    -Wextra
    -Wpedantic
    -Wno-error=unused-parameter
    -Wno-error=unused-variable
    -Wno-error=unused-but-set-variable
    # TODO: this isn't enough
)

if (QPALM_WARNINGS_AS_ERRORS)
    list(APPEND COMMON_WARNINGS -Werror)
endif()

add_library(qpalm_warnings INTERFACE)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(qpalm_warnings INTERFACE ${COMMON_WARNINGS})
else() # TODO: Clang, MSVC
    message(FATAL_ERROR "No known warnings for this compiler")
endif()
