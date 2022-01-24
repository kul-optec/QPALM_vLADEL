find_package(Eigen3 REQUIRED)

add_library(qpalm_cxx
    "src/sparse.cpp"
    "src/qpalm.cpp"
    "include/sparse.hpp"
    "include/qpalm.hpp"
)
target_include_directories(qpalm_cxx
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
target_link_libraries(qpalm_cxx PRIVATE qpalm_warnings)
target_link_libraries(qpalm_cxx PUBLIC qpalm Eigen3::Eigen)
set_target_properties(qpalm_cxx PROPERTIES POSITION_INDEPENDENT_CODE On)