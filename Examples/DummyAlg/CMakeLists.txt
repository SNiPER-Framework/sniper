cmake_minimum_required(VERSION 2.8)

if(ROOT_FOUND)
# Define project
#project(DummyAlg)

#
# build
include_directories(${PROJECT_SOURCE_DIR}/SniperKernel)
include_directories(${PROJECT_SOURCE_DIR}/SniperSvc/RootWriter)
file(GLOB sources src/*.cc)
add_library(DummyAlg SHARED ${sources})
target_link_libraries(DummyAlg SniperKernel RootWriter)

# install
install(TARGETS DummyAlg DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY python/DummyAlg DESTINATION ${CMAKE_INSTALL_PYTHONDIR})
install(FILES share/run.py DESTINATION ${CMAKE_INSTALL_DATADIR}/Examples RENAME run-DummyAlg.py)
endif()
