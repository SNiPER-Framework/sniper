if(USE_PYTHON)
# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperPyUsages SHARED ${sources})
target_link_libraries(SniperPyUsages PRIVATE SniperPythonLib)

# install
install(TARGETS SniperPyUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/SniperPyUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install_and_test(PyUsages
    options/run-PyAndCppAlgs.py
)

endif()
