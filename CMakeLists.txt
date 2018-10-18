cmake_minimum_required(VERSION 2.8)
message(STATUS "cmake version ${CMAKE_VERSION}")
# Enforce an out-of-source builds before anything else
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "This software requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
endif()

# Define project
project(SNiPER)
string(TOLOWER "${PROJECT_NAME}" ${PROJECT_NAME}_TOLOW)
set(${PROJECT_NAME}_VERSION_MAJOR 1)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(${PROJECT_NAME}_VERSION_PATCH )
string(REGEX REPLACE "\\.+$" "" ${PROJECT_NAME}_VERSION 
  ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# ... and configuration options
option(DEBUG      "Turn debug options on" OFF)

# Also https://cmake.org/Wiki/CMake:How_To_Write_Platform_Checks
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# check headers
include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(time.h   HAVE_TIME_H)
check_include_files(dlfcn.h  HAVE_DLFCN_H)
# check functions
# check libraries
# check packages
#include(RequiredPackages)
find_package(UnixCommands)
find_package(Git)
find_package(PythonLibs 2.7)
if(PYTHONLIBS_FOUND)
  find_package(PythonInterp ${PYTHONLIBS_VERSION_STRING})
endif()
find_package(ROOT 5.18)
set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.53 REQUIRED COMPONENTS python)

# now check version information
if(GIT_FOUND)

  # closest tag
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
    OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # it also codes the last package version on this branch
  string(REGEX REPLACE "^v" "" ${PROJECT_NAME}_GIT_VERSION "${${PROJECT_NAME}_GIT_TAG}")
  if("${${PROJECT_NAME}_VERSION_PATCH}" STREQUAL "dev")
    if (NOT ${${PROJECT_NAME}_VERSION} VERSION_GREATER "${${PROJECT_NAME}_GIT_VERSION}")
      message(WARNING "Project version mismatch: cmake=${${PROJECT_NAME}_VERSION} git=${${PROJECT_NAME}_GIT_VERSION}")
    endif()
  else()
    if (NOT ${${PROJECT_NAME}_VERSION} VERSION_EQUAL "${${PROJECT_NAME}_GIT_VERSION}")
      message(WARNING "Project version mismatch: cmake=${${PROJECT_NAME}_VERSION} git=${${PROJECT_NAME}_GIT_VERSION}")
    endif()
  endif()

  # compact form for easy understanding of the current position of HEAD
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags HEAD
    OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_CID
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # calculate number of commits from the beginning
  #  - something like SNV revision number
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --oneline HEAD
    OUTPUT_VARIABLE tmp)
  string(REGEX MATCHALL "[^\n]+\n" tmp "${tmp}")
  list(LENGTH tmp tmp)
  set(${PROJECT_NAME}_GIT_REV "r${tmp}")

  # Do we have any modifications since last commit
  execute_process(
    COMMAND ${GIT_EXECUTABLE} status --short --untracked-files=no
    OUTPUT_VARIABLE tmp)
  if(NOT "${tmp}" STREQUAL "")
    set(${PROJECT_NAME}_GIT_TAG "${${PROJECT_NAME}_GIT_TAG}-M")
    set(${PROJECT_NAME}_GIT_CID "${${PROJECT_NAME}_GIT_CID}-M")
  endif()

  unset(tmp)

endif()

#
#add_definitions(-DHAVE_CONFIG_H)
include_directories(${Boost_INCLUDE_DIRS})
if(ROOT_FOUND)
  add_definitions(-DHAVE_ROOT)
  include_directories(${ROOT_INCLUDE_DIRS})
endif()
if(PYTHONLIBS_FOUND)
  add_definitions(-DHAVE_PYTHON)
  include_directories(${PYTHON_INCLUDE_DIR})
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # APPLE is in around
  add_definitions(-DDARWIN)
  set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # MACOSX_RPATH
  endif()
  execute_process(COMMAND hostname OUTPUT_VARIABLE tmp OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(ENV{HOSTNAME} ${tmp})
  unset(tmp)
endif()
if(WIN32)
  add_definitions(-DWIN32)
endif()
if(DEBUG)
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed -Wl,--no-undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic")
set(datadir "share/sniper")
string(TIMESTAMP CONFIG_DATE)
set(CONFIG_DATE ${CONFIG_DATE} CACHE INTERNAL "Date when build was made")
set(CONFIG_USER $ENV{USER} CACHE INTERNAL "Name who did the build")

#
# build targets
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/project.cmt "project sniper\n\nuse ExternalInterface\n\nbuild_strategy with_installarea\nsetup_strategy no_root\nsetup_strategy no_config\nstructure_strategy without_version_directory\n")
file(COPY cmt/version.cmt DESTINATION cmt)
set(subdirs SniperKernel SniperUtil SniperSvc Examples)
set(modlist)
foreach(dir ${subdirs})
  add_subdirectory(${dir})
endforeach()
# config is after subdirs since we need filename of SniperKernel library
configure_file("${PROJECT_SOURCE_DIR}/${${PROJECT_NAME}_TOLOW}-config.in" "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TOLOW}-config" @ONLY)

#
# install
install(FILES setup.sh DESTINATION .)
install(PROGRAMS "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TOLOW}-config" DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${datadir})
install(DIRECTORY SniperPolicy/cmt/fragments DESTINATION ${datadir}/SniperKernel/cmt)
