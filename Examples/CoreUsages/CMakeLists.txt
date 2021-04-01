# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperCoreUsages SHARED ${sources})
target_include_directories(SniperCoreUsages PRIVATE
    ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(SniperCoreUsages PRIVATE SniperKernel)

# install
install(TARGETS SniperCoreUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install_and_test(CoreUsages
    "LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/SniperKernel:${PROJECT_BINARY_DIR}/Examples/CoreUsages:$ENV{LD_LIBRARY_PATH}"
    options/run-HelloWorld.json
    options/run-SvcAndTool.json
    options/senior-DataIO.json
    options/senior-StopRun.json
)

if(USE_PYTHON)
    install(DIRECTORY python/SniperCoreUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    install(FILES
        options/run-HelloWorld.py
        options/run-SvcAndTool.py
        options/senior-DataIO.py
        options/senior-StopRun.py
        DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
endif()
