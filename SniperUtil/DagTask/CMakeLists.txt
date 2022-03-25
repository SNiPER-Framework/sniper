# build
file(GLOB sources CONFIGURE_DEPENDS lib/*.cc)
add_library(DagTaskLib SHARED ${sources})
target_include_directories(DagTaskLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(DagTaskLib PUBLIC SniperKernel)

# install
install(TARGETS DagTaskLib
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY DagTask DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(USE_PYTHON)
    # build python binding
    file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
    add_library(DagTask SHARED ${sources})
    target_include_directories(DagTask PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    )
    target_link_libraries(DagTask PRIVATE DagTaskLib ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY})

    # install
    install(TARGETS DagTask DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/DagTask)
    install(DIRECTORY python/DagTask DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
endif(USE_PYTHON)
