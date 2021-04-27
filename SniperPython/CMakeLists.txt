# build
file(GLOB sources CONFIGURE_DEPENDS lib/*.cc)
add_library(SniperPythonLib SHARED ${sources})
target_include_directories(SniperPythonLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(SniperPythonLib PUBLIC SniperKernel ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY})

file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
add_library(SniperPython SHARED ${sources})
target_include_directories(SniperPython PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
)
target_link_libraries(SniperPython PRIVATE SniperPythonLib)

# install
install(TARGETS SniperPythonLib
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS SniperPython DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/Sniper)
install(DIRECTORY SniperPython DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES python/SniperPython.py DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(DIRECTORY python/Sniper DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
