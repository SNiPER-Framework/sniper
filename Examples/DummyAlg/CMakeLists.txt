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
install(TARGETS DummyAlg DESTINATION lib)
file(GLOB python python/DummyAlg/*.py share/*.py)
install(FILES ${python} DESTINATION python/DummyAlg)
endif()
