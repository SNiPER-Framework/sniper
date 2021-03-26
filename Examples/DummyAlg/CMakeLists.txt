if(USE_ROOT)
# build
file(GLOB sources src/*.cc)
add_library(DummyAlg SHARED ${sources})
target_link_libraries(DummyAlg SniperKernel RootWriterLib)

# install
install(TARGETS DummyAlg DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/DummyAlg DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES share/run.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples RENAME run-DummyAlg.py)

endif()
