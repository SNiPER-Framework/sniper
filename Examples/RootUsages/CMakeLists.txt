if(USE_ROOT)
# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperRootUsages SHARED ${sources})
target_link_libraries(SniperRootUsages PRIVATE RootWriterLib)

# install
install(TARGETS SniperRootUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install_and_test(RootUsages
    "LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/SniperKernel:${PROJECT_BINARY_DIR}/SniperUtil/RootWriter:${PROJECT_BINARY_DIR}/Examples/RootUsages:$ENV{LD_LIBRARY_PATH}"
    options/run-WriteRootAlg.json
)

if(USE_PYTHON)
    install(FILES options/run-WriteRootAlg.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
    install(DIRECTORY python/SniperRootUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
endif()

endif()
