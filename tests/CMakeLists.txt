include_directories(${CMAKE_SOURCE_DIR}/SniperKernel)

add_executable(TestJSON TestJSON.cc)
target_link_libraries(TestJSON SniperKernel)

add_test(NAME json COMMAND TestJSON)
set_property(TEST json PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$ENV{LD_LIBRARY_PATH})
