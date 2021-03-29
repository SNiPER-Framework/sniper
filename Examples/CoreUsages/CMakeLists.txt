# build
file(GLOB sources src/*.cc)
add_library(SniperCoreUsages SHARED ${sources})
target_link_libraries(SniperCoreUsages SniperKernel)

# install
install(TARGETS SniperCoreUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(USE_PYTHON)
    install(DIRECTORY python/SniperCoreUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    install(FILES options/run-HelloWorld.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
endif()
