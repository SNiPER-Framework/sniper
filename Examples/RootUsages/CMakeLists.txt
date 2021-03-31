if(USE_ROOT)
# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperRootUsages SHARED ${sources})
target_link_libraries(SniperRootUsages PRIVATE RootWriterLib)

# install
install(TARGETS SniperRootUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES options/run-WriteRootAlg.json DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)

if(USE_PYTHON)
    install(FILES options/run-WriteRootAlg.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
    install(DIRECTORY python/SniperRootUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
endif()

endif()
