# build
file(GLOB sources lib/*.cc)
add_library(RootWriterLib SHARED ${sources})
target_include_directories(RootWriterLib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR} ${ROOT_INCLUDE_DIRS}
)
target_link_libraries(RootWriterLib SniperKernel ${ROOT_LIBRARIES})

file(GLOB sources src/*.cc)
add_library(RootWriter SHARED ${sources})
target_include_directories(RootWriter PUBLIC
    ${CMAKE_CURRENT_LIST_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
)
if(NOT ROOT_PyROOT_LIBRARY)
    string(REPLACE Core PyROOT ROOT_PyROOT_LIBRARY ${ROOT_Core_LIBRARY})
endif()
if(NOT EXISTS ${ROOT_PyROOT_LIBRARY} )
    string(REPLACE Core ROOTTPython ROOT_PyROOT_LIBRARY ${ROOT_Core_LIBRARY})
endif()
if(NOT EXISTS ${ROOT_PyROOT_LIBRARY} )
    message(FATAL_ERROR "missing PyROOT/ROOTTPython library")
endif()
target_link_libraries(RootWriter RootWriterLib ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY} ${ROOT_PyROOT_LIBRARY})

# install
install(TARGETS RootWriterLib DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS RootWriter DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/RootWriter)
install(DIRECTORY RootWriter DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY python/RootWriter DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
