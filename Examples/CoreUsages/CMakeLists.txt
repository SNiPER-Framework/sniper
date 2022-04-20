# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperCoreUsages SHARED ${sources})
target_include_directories(SniperCoreUsages PRIVATE
    ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(SniperCoreUsages PRIVATE SniperKernel)

# install
install(TARGETS SniperCoreUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install_and_test(CoreUsages
    options/run-AlgDepGraph.json
    options/run-HelloWorld.json
    options/run-SvcAndTool.json
    options/run-TimerUsage.json
    options/senior-DataIO.json
    options/senior-StopRun.json
    options/senior-SubTasks.json
    options/senior-1Svc2Tasks.json
)

if(USE_PYTHON)
    install(FILES python/SniperCoreUsages.py DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    install_and_test(CoreUsages
        options/run-AlgDepGraph.py
        options/run-HelloWorld.py
        options/run-SvcAndTool.py
        options/run-TimerUsage.py
        options/senior-DataIO.py
        options/senior-StopRun.py
        options/senior-SubTasks.py
        options/senior-1Svc2Tasks.py
    )
endif()
