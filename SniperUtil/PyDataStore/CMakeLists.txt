cmake_minimum_required(VERSION 2.8)

# Define project
#project(PyDataStore)

#
# build
include_directories(${PROJECT_SOURCE_DIR}/SniperKernel)
include_directories(${PROJECT_SOURCE_DIR}/SniperUtil/PyDataStore)
file(GLOB sources src/*.cc)
add_library(PyDataStore SHARED ${sources})
target_link_libraries(PyDataStore SniperPython)

# install
install(TARGETS PyDataStore DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/PyDataStore DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
