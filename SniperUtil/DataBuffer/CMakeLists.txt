cmake_minimum_required(VERSION 2.8)

# Define project
#project(DataBuffer)

#
# build

# install
file(GLOB headers DataBuffer/*.h)
install(FILES ${headers} DESTINATION include/DataBuffer)
