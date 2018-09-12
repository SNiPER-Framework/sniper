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
set(ROOT_PyROOT_LIBRARY /usr/lib64/root/libPyROOT.so)
target_link_libraries(RootWriter SniperKernel ${ROOT_Core_LIBRARY} ${ROOT_RIO_LIBRARY} ${ROOT_Tree_LIBRARY} ${ROOT_PyROOT_LIBRARY} ${PYTHON_LIBRARY} boost_python)

# install
install(TARGETS RootWriter DESTINATION lib)
file(GLOB headers RootWriter/*.h)
install(FILES ${headers} DESTINATION include/RootWriter)
file(GLOB python python/RootWriter/*.py)
install(FILES ${python} DESTINATION python/RootWriter)
endif()
