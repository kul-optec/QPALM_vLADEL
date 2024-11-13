function(pybind11_stubgen target)

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set_target_properties(${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "$<CONFIG>/${PY_BUILD_CMAKE_MODULE_NAME}")
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E touch ${PY_BUILD_CMAKE_MODULE_NAME}/__init__.py
        COMMAND ${Python3_EXECUTABLE} -m pybind11_stubgen
                ${PY_BUILD_CMAKE_MODULE_NAME}.$<TARGET_FILE_BASE_NAME:${target}>
                --numpy-array-remove-parameters
                --ignore-invalid-expressions \\\\?
                --ignore-unresolved-names \"m|n\"
                --exit-code
                -o ${CMAKE_CURRENT_BINARY_DIR}/stubs/$<TARGET_FILE_BASE_NAME:${target}>
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${target}>/..)

endfunction()

function(pybind11_stubgen_install target)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stubs/$<TARGET_FILE_BASE_NAME:${target}>/${PY_BUILD_CMAKE_MODULE_NAME}
        EXCLUDE_FROM_ALL
        COMPONENT python_stubs
        DESTINATION .
        FILES_MATCHING REGEX "\.pyi$")

endfunction()
