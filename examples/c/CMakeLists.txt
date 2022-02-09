add_executable(qpalm_demo "qpalm_demo.c")
target_link_libraries(qpalm_demo PRIVATE QPALM::qpalm QPALM::qpalm_lax_warnings)