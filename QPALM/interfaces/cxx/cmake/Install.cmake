include(GNUInstallDirs)

set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/qpalm_cxx")

# Add the qpalm library to the "export-set", install the library files
install(TARGETS qpalm_cxx qpalm_warnings
    EXPORT qpalm_cxxTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        COMPONENT shlib
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" 
        COMPONENT lib)

# Install the header files
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT dev
    FILES_MATCHING REGEX "/.*\.[hti](pp)?$")

# Install the export set for use with the install tree
install(EXPORT qpalm_cxxTargets 
    FILE qpalm_cxxTargets.cmake
    DESTINATION "${INSTALL_CMAKE_DIR}"
        COMPONENT dev
    NAMESPACE ${PROJECT_NAME}::)

# Generate the config file that includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${PROJECT_BINARY_DIR}/qpalm_cxxConfig.cmake"
    INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/qpalm_cxxConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)

# Install the qpalm_cxxConfig.cmake and qpalm_cxxConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}/qpalm_cxxConfig.cmake"
    "${PROJECT_BINARY_DIR}/qpalm_cxxConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}" 
        COMPONENT dev)

# Add all targets to the build tree export set
export(EXPORT qpalm_cxxTargets
    FILE "${PROJECT_BINARY_DIR}/qpalm_cxxTargets.cmake"
    NAMESPACE ${PROJECT_NAME}::)
