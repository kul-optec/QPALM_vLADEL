add_executable(qpalm_demo_cxx "qpalm_demo.cpp")
target_link_libraries(qpalm_demo_cxx PRIVATE QPALM::qpalm_cxx QPALM::qpalm_warnings)