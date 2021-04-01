if(USE_ROOT)
    # build cxx lib
    add_library(RootWriterLib SHARED lib/RootWriter.cc)
    target_include_directories(RootWriterLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> ${ROOT_INCLUDE_DIRS}
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(RootWriterLib PUBLIC SniperKernel ${ROOT_LIBRARIES})
    
    # install
    install(TARGETS RootWriterLib
        EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY RootWriter DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    
    if(USE_PYTHON)
        # build python binding
        add_library(RootWriter SHARED src/RootWriterBinding.cc)
        target_include_directories(RootWriter PRIVATE
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
        target_link_libraries(RootWriter PRIVATE RootWriterLib ${PYTHON_LIBRARY} ${BOOST_PYTHON_LIBRARY} ${ROOT_PyROOT_LIBRARY})
        
        # install
        install(TARGETS RootWriter DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/RootWriter)
        install(DIRECTORY python/RootWriter DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
    endif(USE_PYTHON)
endif(USE_ROOT)
