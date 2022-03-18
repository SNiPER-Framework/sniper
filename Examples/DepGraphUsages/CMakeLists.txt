# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(DepGraphUsages SHARED ${sources})
target_include_directories(DepGraphUsages PRIVATE
    ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(DepGraphUsages PRIVATE SniperKernel)

# install
install(TARGETS DepGraphUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install_and_test(CoreUsages
    options/run-AlgDepGraph.json
)

if(USE_PYTHON)
    install(FILES python/DepGraphUsages.py DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    install_and_test(CoreUsages
        options/run-AlgDepGraph.py
    )
endif()
