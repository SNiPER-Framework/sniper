cmake_minimum_required(VERSION 2.8)

# Define project
#project(HelloWorld)

#
# build
include_directories(${PROJECT_SOURCE_DIR}/SniperKernel)
file(GLOB sources src/*.cc)
add_library(HelloWorld SHARED ${sources})
target_link_libraries(HelloWorld SniperKernel)

# install
install(TARGETS HelloWorld DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/HelloWorld DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES share/run.py DESTINATION ${CMAKE_INSTALL_DATADIR}/Examples RENAME run-HelloWorld.py)
