cmake_minimum_required(VERSION 2.8)

# Define project
#project(SniperKernel)

#
# build
include_directories(${PROJECT_SOURCE_DIR}/SniperKernel)
file(GLOB sources src/*.cc)
add_library(SniperKernel SHARED ${sources})
target_link_libraries(SniperKernel dl ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR}/SniperKernel/src)
file(GLOB sources src/binding/*.cc)
add_library(SniperPython SHARED ${sources})
target_link_libraries(SniperPython SniperKernel)

set(file)
cmake_policy(PUSH)
  if(POLICY CMP0026)
    cmake_policy(SET CMP0026 OLD)
  endif()
  get_target_property(file SniperKernel LOCATION)
cmake_policy(POP)
get_filename_component(file ${file} NAME)
set(SniperKernel_LIB_NAME ${file} PARENT_SCOPE)
if(BUILD_CMT_PROJECT)
  file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "
package SniperKernel

use SniperPolicy *

include_path none

#macro SniperKernel_cppflags \"-std=c++11 -fPIC \"
macro SniperKernel_linkopts \" -Wl,--as-needed -Wl,--no-undefined ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${file} ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES} \"

")
endif()
unset(file)

# install
install(TARGETS SniperKernel DESTINATION ${CMAKE_INSTALL_LIBDIR})
file(GLOB headers SniperKernel/*.h)
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SniperKernel)
install(TARGETS SniperPython DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/Sniper)
install(DIRECTORY python/Sniper DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES share/HelloTask.py DESTINATION ${CMAKE_INSTALL_DATADIR}/Examples RENAME run-HelloTask.py)
if(BUILD_CMT_PROJECT)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${CMAKE_INSTALL_CMTPROJECTDIR}/SniperKernel)
endif()
