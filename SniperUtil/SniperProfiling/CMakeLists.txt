# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperProfilingLib SHARED ${sources})
target_include_directories(SniperProfilingLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(SniperProfilingLib PUBLIC SniperKernel)

# install
install(TARGETS SniperProfilingLib
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY SniperProfiling DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(USE_PYTHON)
    # install
    install(DIRECTORY python/SniperProfiling DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
endif(USE_PYTHON)
