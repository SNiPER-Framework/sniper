# build
file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperCoreUsages SHARED ${sources})
target_include_directories(SniperCoreUsages PRIVATE
    ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(SniperCoreUsages PRIVATE SniperKernel)

# install
install(TARGETS SniperCoreUsages DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES
    options/run-HelloWorld.json
    options/run-DummyAlg.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
if(USE_PYTHON)
    install(DIRECTORY python/SniperCoreUsages DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    install(FILES options/run-HelloWorld.py DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
endif()

# test
add_test(NAME HelloWorld
    COMMAND sniper.exe ${CMAKE_CURRENT_LIST_DIR}/options/run-HelloWorld.json)
add_test(NAME DummyAlg
    COMMAND sniper.exe ${CMAKE_CURRENT_LIST_DIR}/options/run-DummyAlg.json)
set_tests_properties(HelloWorld DummyAlg PROPERTIES
    ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/SniperKernel:${PROJECT_BINARY_DIR}/Examples/CoreUsages:$LD_LIBRARY_PATH"
    FAIL_REGULAR_EXPRESSION "NotFound;Failed"
    PASS_REGULAR_EXPRESSION "events processed 5.* Terminated Successfully"
)
