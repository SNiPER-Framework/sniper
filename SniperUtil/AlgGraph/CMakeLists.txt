# build
file(GLOB sources CONFIGURE_DEPENDS lib/*.cc)
add_library(AlgGraphLib SHARED ${sources})
target_include_directories(AlgGraphLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(AlgGraphLib PUBLIC SniperKernel)

# install
install(TARGETS AlgGraphLib
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY AlgGraph DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(USE_PYTHON)
    # build python binding
    file(GLOB sources CONFIGURE_DEPENDS src/*.cc)
    add_library(AlgGraph SHARED ${sources})
    target_include_directories(AlgGraph PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    )
    target_link_libraries(AlgGraph PRIVATE AlgGraphLib ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY})

    # install
    install(TARGETS AlgGraph DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/AlgGraph)
    install(DIRECTORY python/AlgGraph DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
endif(USE_PYTHON)
