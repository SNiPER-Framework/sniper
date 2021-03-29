if(USE_PYTHON)
# build
file(GLOB sources src/*.cc)
add_library(SniperPyUsages SHARED ${sources})
target_link_libraries(SniperPyUsages SniperKernel SniperPythonLib)

# install
install(TARGETS SniperPyUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/SniperPyUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES options/run-PyAndCppAlgs.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)

endif()
