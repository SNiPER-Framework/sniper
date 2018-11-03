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
get_target_property(file SniperKernel LOCATION)
get_filename_component(file ${file} NAME)
set(SniperKernel_LIB_NAME ${file} PARENT_SCOPE)
file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "
package SniperKernel

use SniperPolicy *

include_path none

#macro SniperKernel_cppflags \"-std=c++11 -fPIC \"
macro SniperKernel_linkopts \" -Wl,--as-needed -Wl,--no-undefined ${CMAKE_INSTALL_PREFIX}/lib/${file} ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES} \"

")
unset(file)

# install
install(TARGETS SniperKernel SniperPython DESTINATION lib)
file(GLOB headers SniperKernel/*.h)
install(FILES ${headers} DESTINATION include/SniperKernel)
file(GLOB python python/Sniper/*.py)
install(FILES ${python} DESTINATION python/Sniper)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${datadir}/SniperKernel)
