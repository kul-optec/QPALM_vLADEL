find_package(Eigen3 REQUIRED)

add_library(qpalm_cxx
    "src/sparse.cpp"
    "src/qpalm.cpp"
    "include/sparse.hpp"
    "include/qpalm.hpp"
)
target_include_directories(qpalm_cxx
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
target_compile_features(qpalm_cxx PUBLIC cxx_std_17)
target_link_libraries(qpalm_cxx PRIVATE qpalm_warnings)
target_link_libraries(qpalm_cxx PUBLIC ${PROJECT_NAME}::qpalm Eigen3::Eigen)
add_library(${PROJECT_NAME}::qpalm_cxx ALIAS qpalm_cxx)

include(cmake/Install.cmake)