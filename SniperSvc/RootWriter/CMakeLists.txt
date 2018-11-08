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
if(NOT ROOT_PyROOT_LIBRARY)
  # sometimes this library isn't referenced automatically
  get_filename_component(dir ${ROOT_Core_LIBRARY} DIRECTORY)
  find_library(ROOT_PyROOT_LIBRARY NAMES PyROOT PATHS ${dir} NO_DEFAULT_PATH)
endif()
# To be honest, we should check that both ROOT and Boost use the same python,
# because otherwise it won't work.
target_link_libraries(RootWriter SniperKernel ${ROOT_Core_LIBRARY} ${ROOT_RIO_LIBRARY} ${ROOT_Tree_LIBRARY} ${ROOT_PyROOT_LIBRARY} ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES})
set(file)
get_target_property(file RootWriter LOCATION)
get_filename_component(file ${file} NAME)
file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "
package RootWriter

use SniperKernel v*

include_path none
include_dirs \"${ROOT_INCLUDE_DIRS}\"

macro_append RootWriter_cppflags \" -Wno-long-long \"
macro_append RootWriter_linkopts \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${file} ${ROOT_Tree_LIBRARY} ${ROOT_Core_LIBRARY} \"
")

# install
install(TARGETS RootWriter DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES RootWriter/RootWriter.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RootWriter)
install(DIRECTORY python/RootWriter DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${CMAKE_INSTALL_CMTPROJECTDIR}/SniperSvc/RootWriter)

endif()
