if(USE_ROOT)
# build
file(GLOB sources src/*.cc)
add_library(SniperRootUsages SHARED ${sources})
target_link_libraries(SniperRootUsages SniperKernel RootWriterLib)

# install
install(TARGETS SniperRootUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/SniperRootUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES options/run-WriteRootAlg.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)

endif()
