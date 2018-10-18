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
install(TARGETS HelloWorld DESTINATION lib)
file(GLOB python python/HelloWorld/*.py share/*.py)
install(FILES ${python} DESTINATION python/HelloWorld)
