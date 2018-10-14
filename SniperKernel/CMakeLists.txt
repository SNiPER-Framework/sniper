cmake_minimum_required(VERSION 2.8)

# Define project
#project(SniperKernel)

#
# build
include_directories(${PROJECT_SOURCE_DIR}/SniperKernel)
file(GLOB sources src/*.cc)
add_library(SniperKernel SHARED ${sources})
target_link_libraries(SniperKernel dl ${PYTHON_LIBRARY} boost_python)

include_directories(${PROJECT_SOURCE_DIR}/SniperKernel/src)
file(GLOB sources src/binding/*.cc)
add_library(SniperPython SHARED ${sources})
target_link_libraries(SniperPython SniperKernel)

get_target_property(_file SniperKernel LOCATION)
get_filename_component(_file ${_file} NAME)
set(SniperKernel_LIB_NAME ${_file} PARENT_SCOPE)
file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
set(req_head "
package SniperKernel

macro SniperKernel_home \"${CMAKE_INSTALL_PREFIX}\"
macro SniperKernel_root \"$(SniperKernel_home)/share/sniper/SniperKernel\"

macro_remove    cppflags        \"-ansi -pedantic \"
macro_prepend   cppflags        \"-std=c++11 -fPIC \"

include_path none
include_dirs \"$(SniperKernel_home)/include\"
include_dirs \"${Boost_INCLUDE_DIR}\"
include_dirs \"${PYTHON_INCLUDE_DIR}\"

#macro SniperKernel_cppflags \"-std=c++11 -fPIC \"
macro SniperKernel_linkopts \"-fPIC -Wl,--as-needed -Wl,--no-undefined $(SniperKernel_home)/lib/${_file} ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARY} \"

path_prepend PYTHONPATH \"$(SniperKernel_home)/python:$(SniperKernel_home)/lib\"
path_prepend LD_LIBRARY_PATH \"$(SniperKernel_home)/lib\"

#
# from SniperPolicy
#
")
file(READ ${PROJECT_SOURCE_DIR}/SniperPolicy/cmt/requirements _rules)
string(FIND "${_rules}" "pattern" _offset)
string(SUBSTRING "${_rules}" "${_offset}" "-1" _rules)
string(REPLACE "SniperPolicy_" "SniperKernel_" req_rules "${_rules}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "${req_head}${req_rules}")
unset(_file)
unset(_rules)
unset(_offset)

# install
install(TARGETS SniperKernel SniperPython DESTINATION lib)
file(GLOB headers SniperKernel/*.h)
install(FILES ${headers} DESTINATION include/SniperKernel)
file(GLOB python python/Sniper/*.py)
install(FILES ${python} DESTINATION python/Sniper)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${datadir}/SniperKernel)
