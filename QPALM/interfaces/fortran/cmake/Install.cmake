include(GNUInstallDirs)

set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/QPALM_fortran")

# Add the qpalm library to the "export-set", install the library files
install(TARGETS qpalm_fortran
    EXPORT QPALM_fortranTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT shlib
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        COMPONENT shlib
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" 
        COMPONENT lib)

# Install the header files
install(DIRECTORY "${QPALM_FORTRAN_MODULE_DIR}/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT dev
    FILES_MATCHING REGEX "/.*\.mod$")

# Install the export set for use with the install tree
install(EXPORT QPALM_fortranTargets 
    FILE QPALM_fortranTargets.cmake
    DESTINATION "${INSTALL_CMAKE_DIR}"
        COMPONENT dev
    NAMESPACE ${PROJECT_NAME}::)

# Generate the config file that includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${PROJECT_BINARY_DIR}/QPALM_fortranConfig.cmake"
    INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/QPALM_fortranConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)

# Install the QPALM_fortranConfig.cmake and QPALM_fortranConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}/QPALM_fortranConfig.cmake"
    "${PROJECT_BINARY_DIR}/QPALM_fortranConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}" 
        COMPONENT dev)

# Add all targets to the build tree export set
export(EXPORT QPALM_fortranTargets
    FILE "${PROJECT_BINARY_DIR}/QPALM_fortranTargets.cmake"
    NAMESPACE ${PROJECT_NAME}::)
