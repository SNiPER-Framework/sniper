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

# install
install(TARGETS SniperKernel SniperPython DESTINATION lib)
file(GLOB headers SniperKernel/*.h)
install(FILES ${headers} DESTINATION include/SniperKernel)
file(GLOB python python/Sniper/*.py)
install(FILES ${python} DESTINATION python/Sniper)
