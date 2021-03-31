# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperKernel SHARED ${sources})
target_include_directories(SniperKernel PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(SniperKernel PUBLIC dl)

# install
install(TARGETS SniperKernel
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY SniperKernel DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
