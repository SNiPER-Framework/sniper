if(USE_PYTHON)
# build
file(GLOB sources src/*.cc)
add_library(HelloWorld SHARED ${sources})
target_link_libraries(HelloWorld SniperKernel SniperPythonLib)

# install
install(TARGETS HelloWorld DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/HelloWorld DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES share/run.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples RENAME run-HelloWorld.py)

endif()