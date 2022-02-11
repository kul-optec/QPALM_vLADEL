add_executable(qpalm_qps 
               src/qpalm_qps.c
               src/qps_conversion.c
               src/index_hash.c)

target_include_directories(qpalm_qps PRIVATE include)
target_link_libraries(qpalm_qps PRIVATE QPALM::qpalm qpalm_lax_warnings)

include(GNUInstallDirs)
install(TARGETS qpalm_qps
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT bin)
