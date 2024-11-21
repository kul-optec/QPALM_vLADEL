include(GNUInstallDirs)

set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/QPALM")

# Add the qpalm library to the "export-set", install the library files
install(TARGETS qpalm_cxx qpalm_warnings
    EXPORT QPALM_cxxTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT shlib
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        COMPONENT shlib
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" 
        COMPONENT lib)

# Install the header files
install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT dev
    FILES_MATCHING REGEX "/.*\.[hti](pp)?$")

# Install the export set for use with the install tree
install(EXPORT QPALM_cxxTargets 
    FILE QPALM_component_cxxTargets.cmake
    DESTINATION "${INSTALL_CMAKE_DIR}"
        COMPONENT dev
    NAMESPACE ${PROJECT_NAME}::)

# Generate the config file that includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${PROJECT_BINARY_DIR}/QPALM_component_cxxConfig.cmake"
    INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Install the QPALM_component_cxxConfig.cmake
install(FILES
    "${PROJECT_BINARY_DIR}/QPALM_component_cxxConfig.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}" 
        COMPONENT dev)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/legacy-QPALM_cxxConfig.cmake"
    RENAME "QPALM_cxxConfig.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}_cxx" 
        COMPONENT dev)

# Add all targets to the build tree export set
export(EXPORT QPALM_cxxTargets
    FILE "${PROJECT_BINARY_DIR}/QPALM_component_cxxTargets.cmake"
    NAMESPACE ${PROJECT_NAME}::)
