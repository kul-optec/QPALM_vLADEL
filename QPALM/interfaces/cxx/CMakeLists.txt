find_package(Eigen3 REQUIRED)

add_library(qpalm_cxx
    "src/sparse.cpp"
    "src/qpalm.cpp"
    "include/qpalm/sparse.hpp"
    "include/qpalm.hpp"
)
target_include_directories(qpalm_cxx
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
target_compile_features(qpalm_cxx PUBLIC cxx_std_17)
target_link_libraries(qpalm_cxx PRIVATE qpalm_warnings)
target_link_libraries(qpalm_cxx PUBLIC ${PROJECT_NAME}::qpalm Eigen3::Eigen)
set_target_properties(qpalm_cxx PROPERTIES 
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden)
include(GenerateExportHeader)
generate_export_header(qpalm_cxx
    EXPORT_FILE_NAME export/qpalm/qpalm_cxx-export.hpp)
target_include_directories(qpalm_cxx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/export>)
set_target_properties(qpalm_cxx PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
add_library(${PROJECT_NAME}::qpalm_cxx ALIAS qpalm_cxx)

include(cmake/Install.cmake)