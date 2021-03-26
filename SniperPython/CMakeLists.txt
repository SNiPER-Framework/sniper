# build
file(GLOB sources lib/*.cc)
add_library(SniperPythonLib SHARED ${sources})
target_include_directories(SniperPythonLib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
)
target_link_libraries(SniperPythonLib SniperKernel ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY})

file(GLOB sources src/*.cc)
add_library(SniperPython SHARED ${sources})
target_include_directories(SniperPython PUBLIC
    ${CMAKE_CURRENT_LIST_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
)
target_link_libraries(SniperPython SniperPythonLib SniperKernel ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY})

# install
install(TARGETS SniperPythonLib DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS SniperPython DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/Sniper)
install(DIRECTORY SniperPython DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY python/SniperPython DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(DIRECTORY python/Sniper DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
#install(FILES share/HelloTask.py DESTINATION ${CMAKE_INSTALL_DATADIR}/Examples RENAME run-HelloTask.py)