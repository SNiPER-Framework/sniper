if(USE_ROOT)
# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperRootUsages SHARED ${sources})
target_link_libraries(SniperRootUsages PRIVATE RootWriterLib)

# install
install(TARGETS SniperRootUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install_and_test(RootUsages
    options/run-WriteRootAlg.json
)

if(USE_PYTHON)
    install(FILES python/SniperRootUsages.py DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    install_and_test(RootUsages
        options/run-WriteRootAlg.py
    )
endif()

endif()
