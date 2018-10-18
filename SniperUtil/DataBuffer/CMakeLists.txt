cmake_minimum_required(VERSION 2.8)

# Define project
#project(DataBuffer)

#
# build
file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "package DataBuffer\n\nuse SniperKernel v*\n")

# install
install(FILES DataBuffer/DataBuffer.h DESTINATION include/DataBuffer)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${datadir}/DataBuffer)
