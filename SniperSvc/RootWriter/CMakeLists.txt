cmake_minimum_required(VERSION 2.8)

if(ROOT_FOUND)
# Define project
#project(RootWriter)

#
# build
include_directories(${PROJECT_SOURCE_DIR}/SniperKernel)
include_directories(${PROJECT_SOURCE_DIR}/SniperSvc/RootWriter)
file(GLOB sources src/*.cc binding/RootWriterBinding.cc)
add_library(RootWriter SHARED ${sources})
set(ROOT_PyROOT_LIBRARY ${ROOT_LIBRARY_DIR}/libPyROOT.so)
target_link_libraries(RootWriter SniperKernel ${ROOT_Core_LIBRARY} ${ROOT_RIO_LIBRARY} ${ROOT_Tree_LIBRARY} ${ROOT_PyROOT_LIBRARY} ${PYTHON_LIBRARY} boost_python)
file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "
package RootWriter

use SniperKernel v*

macro_append RootWriter_cppflags \"`root-config --cflags`\"
macro_append RootWriter_cppflags \" -Wno-long-long \"
#macro_append RootWriter_linkopts \"`root-config --libs`\"
")

# install
install(TARGETS RootWriter DESTINATION lib)
install(FILES RootWriter/RootWriter.h DESTINATION include/RootWriter)
file(GLOB python python/RootWriter/*.py)
install(FILES ${python} DESTINATION python/RootWriter)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${datadir}/RootWriter)

endif()
