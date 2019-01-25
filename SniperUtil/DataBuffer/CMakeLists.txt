cmake_minimum_required(VERSION 2.8)

# Define project
#project(DataBuffer)

#
# build
if(BUILD_CMT_PROJECT)
  file(COPY cmt/version.cmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmt)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/requirements "package DataBuffer\n\nuse SniperKernel v*\n")
endif()

# install
install(FILES DataBuffer/DataBuffer.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/DataBuffer)
if(BUILD_CMT_PROJECT)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${CMAKE_INSTALL_CMTPROJECTDIR}/SniperUtil/DataBuffer)
endif()
