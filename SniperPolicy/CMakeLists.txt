cmake_minimum_required(VERSION 2.8)

# Define project
#project(SniperPolicy)

#
# build
file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
set(req_head "
package SniperPolicy

macro_remove    cppflags        \"-ansi -pedantic \"
macro_prepend   cppflags        \"-std=c++11 -fPIC \"

include_path none
include_dirs \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}\"
include_dirs \"${Boost_INCLUDE_DIR}\"
include_dirs \"${PYTHON_INCLUDE_DIR}\"

path_prepend PYTHONPATH \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PYTHONDIR}\"
path_prepend LD_LIBRARY_PATH \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\"

")
file(READ cmt/requirements _rules)
string(FIND "${_rules}" "pattern" _offset)
string(SUBSTRING "${_rules}" "${_offset}" "-1" req_rules)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "${req_head}${req_rules}")
unset(_rules)
unset(_offset)

# install
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${CMAKE_INSTALL_CMTPROJECTDIR}/SniperPolicy)
install(DIRECTORY cmt/fragments DESTINATION ${CMAKE_INSTALL_CMTPROJECTDIR}/SniperPolicy/cmt)
