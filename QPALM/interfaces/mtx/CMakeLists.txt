add_executable(qpalm_mtx qpalm_mtx.c)
target_link_libraries(qpalm_mtx PRIVATE QPALM::qpalm qpalm_lax_warnings)

include(GNUInstallDirs)
install(TARGETS qpalm_mtx
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT bin)
