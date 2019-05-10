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
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# ... and configuration options
option(DEBUG      "Turn debug options on" OFF)
set(CMT_SEARCH_DIRS "Externals" CACHE STRING "Set a path for packages location for CMT" )
option(USE_SIMPLE_DIRS "Use simplified directory structure" OFF)
option(BUILD_CMT_PROJECT "Create a project to use sniper in CMT system" ON)

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
include(CMakePrintHelpers)
find_package(UnixCommands)
find_package(Git)
find_package(CMT)

find_package(PythonLibs 2.7 REQUIRED)
if(PYTHONLIBS_FOUND)
  find_package(PythonInterp ${PYTHONLIBS_VERSION_STRING} EXACT)
else()
  cmt_find_package(Python 2.7 PATHS ${CMT_SEARCH_DIRS})
  if(Python_FOUND AND NOT Python_DIR)
    # fix CMT results
    set(PYTHONLIBS_FOUND TRUE)
    set(PYTHON_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
    set(PYTHON_LIBRARIES ${Python_LIBRARIES})
    foreach(file ${Python_LIBRARIES})
      get_filename_component(name ${file} NAME)
      if(${name} MATCHES "^lib(python([0-9.]+)).(so|a|dylib)")
        set(PYTHON_LIBRARY ${file})
        set(name ${CMAKE_MATCH_1})
        get_filename_component(dir ${file} DIRECTORY)
        if(EXISTS ${dir}/../bin/${name})
          set(PYTHONINTERP_FOUND TRUE)
          get_filename_component(file ${dir}/../bin/${name} ABSOLUTE)
          set(PYTHON_EXECUTABLE ${file} CACHE FILEPATH "Path to a file.")
          mark_as_advanced(PYTHON_EXECUTABLE)
          message(STATUS "Found PythonInterp: ${PYTHON_EXECUTABLE}")
        endif()
      endif()
    endforeach()
  endif()
endif()
if(NOT PYTHONLIBS_FOUND)
  message(FATAL_ERROR "Could NOT find Python\n(Required is at least version \"2.7\")")
endif()
#cmake_print_variables(PYTHON_INCLUDE_DIRS PYTHON_LIBRARY_DIRS PYTHON_LIBRARIES PYTHON_EXECUTABLE)

set(Boost_USE_MULTITHREADED OFF)
set(BOOST_MIN_VERSION "1.67")
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
if(NOT Boost_FOUND)
  cmt_find_package(Boost ${BOOST_MIN_VERSION} REQUIRED PATHS ${CMT_SEARCH_DIRS})
endif()
if(Boost_FOUND AND NOT Boost_PYTHON_LIBRARY)
  foreach(lib ${Boost_LIBRARIES})
    if(${lib} MATCHES "/libboost_python([0-9.]+).(so|a|dylib)")
      set(Boost_PYTHON_FOUND TRUE)
      set(Boost_PYTHON_LIBRARY ${lib} CACHE FILEPATH "Path to a library.")
      mark_as_advanced(Boost_PYTHON_LIBRARY)
    endif()
  endforeach()
  unset(lib)
endif()

if(NOT Boost_FOUND)
  message(FATAL_ERROR "Could NOT find Boost\n(Required is at least version \"${BOOST_MIN_VERSION}\")")
endif()
#cmake_print_variables(Boost_INCLUDE_DIRS Boost_LIBRARY_DIRS Boost_PYTHON_LIBRARY Boost_LIBRARIES)

find_package(ROOT 5.18)
if(ROOT_FOUND)
  # fix FindROOT results
  if(${ROOT_LIBRARY_DIR})
    list(APPEND ROOT_LIBRARY_DIRS ${ROOT_LIBRARY_DIR})
  endif()
else()
  cmt_find_package(ROOT 5.18 PATHS ${CMT_SEARCH_DIRS})
  if(ROOT_FOUND AND NOT ROOT_DIR)
    # fix CMT results
    foreach(lib ${ROOT_LIBRARIES})
      if(${lib} MATCHES "lib([A-Z].+).so")
        set(ROOT_${CMAKE_MATCH_1}_LIBRARY ${lib} CACHE FILEPATH "Path to a file.")
        mark_as_advanced(ROOT_${CMAKE_MATCH_1}_LIBRARY)
      endif()
    endforeach()
    foreach(dir ${ROOT_LIBRARY_DIRS})
      if(EXISTS ${dir}/../bin/root)
        get_filename_component(file ${dir}/../bin/root ABSOLUTE)
        set(ROOT_EXECUTABLE ${file} CACHE FILEPATH "Path to a file.")
        mark_as_advanced(ROOT_EXECUTABLE)
      endif()
    endforeach()
  endif()
endif()
#cmake_print_variables(ROOT_INCLUDE_DIRS ROOT_LIBRARY_DIRS ROOT_Core_LIBRARY ROOT_EXECUTABLE)

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

# Let's define installation directories
if(USE_SIMPLE_DIRS)
  set(CMAKE_INSTALL_BINDIR "bin")
  set(CMAKE_INSTALL_LIBDIR "lib")
  set(CMAKE_INSTALL_INCLUDEDIR "include")
  set(CMAKE_INSTALL_PYTHONDIR "python")
  set(CMAKE_INSTALL_DATADIR "share/${${PROJECT_NAME}_TOLOW}")
  set(CMAKE_INSTALL_CMTPROJECTDIR ${CMAKE_INSTALL_DATADIR})
else()
  include(GNUInstallDirs)
  if(NOT CMAKE_CACHEFILE_DIR)
    # we don't have cache, let's override defaults
    #set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/${${PROJECT_NAME}_TOLOW}" CACHE PATH "C/C++ header files (include/PROJECT_NAME)" FORCE)
    set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/${${PROJECT_NAME}_TOLOW}-${${PROJECT_NAME}_VERSION}" CACHE PATH "documentation root (DATAROOTDIR/doc/PROJECT_NAME)" FORCE)
    set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}/${${PROJECT_NAME}_TOLOW}" CACHE PATH "read-only architecture-independent data (DATAROOTDIR)" FORCE)
  endif()
  if(WIN32)
    set(python_site "Lib/site-packages")
  else()
    if(PYTHON_EXECUTABLE)
      execute_process(
        COMMAND ${PYTHON_EXECUTABLE} "-c" "import sys; print \"python%d.%d\" % (sys.version_info[0:2]);"
        OUTPUT_VARIABLE python
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
      set(python "python")
    endif()
    set(python_site "${CMAKE_INSTALL_LIBDIR}/${python}/site-packages")
  endif()
  if(NOT CMAKE_INSTALL_PYTHONDIR)
    set(CMAKE_INSTALL_PYTHONDIR "${python_site}" CACHE PATH "python modules (LIBDIR/pythonX.Y/site-packages)")
  endif()
  if(NOT CMAKE_INSTALL_CMTPROJECTDIR)
    set(CMAKE_INSTALL_CMTPROJECTDIR "${CMAKE_INSTALL_DATADIR}/cmt-project/${${PROJECT_NAME}_TOLOW}" CACHE PATH "CMT project for devel (DATADIR/cmt-project/PROJECT_NAME)" FORCE)
  endif()
endif()
cmake_print_variables(CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_DATADIR CMAKE_INSTALL_PYTHONDIR CMAKE_INSTALL_CMTPROJECTDIR)

#
#add_definitions(-DHAVE_CONFIG_H)
include_directories(${Boost_INCLUDE_DIRS})
if(PYTHONLIBS_FOUND)
  add_definitions(-DHAVE_PYTHON)
  include_directories(${PYTHON_INCLUDE_DIRS})
endif()
if(ROOT_FOUND)
  add_definitions(-DHAVE_ROOT)
  include_directories(${ROOT_INCLUDE_DIRS})
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # APPLE is in around
  add_definitions(-DDARWIN)
  set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_MACOSX_RPATH 1)
  execute_process(COMMAND hostname OUTPUT_VARIABLE tmp OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(ENV{HOSTNAME} ${tmp})
#  set(CMAKE_SHARED_LIBRARY_SUFFIX .so)
  unset(tmp)
endif()
if(WIN32)
  add_definitions(-DWIN32)
endif()
if(DEBUG)
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed -Wl,--no-undefined")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
endif()
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic")
string(TIMESTAMP CONFIG_DATE)
set(CONFIG_DATE ${CONFIG_DATE} CACHE INTERNAL "Date when build was made")
set(CONFIG_USER $ENV{USER} CACHE INTERNAL "Name who did the build")

#
# build targets
set(subdirs SniperKernel SniperPolicy SniperUtil SniperSvc Examples)
set(modlist)
foreach(dir ${subdirs})
  add_subdirectory(${dir})
endforeach()
# config is after subdirs since we need filename of SniperKernel library
get_filename_component(_CMTPROJECTPATH "${CMAKE_INSTALL_CMTPROJECTDIR}" DIRECTORY)
# actually CMT expects _parent_ directory of CMTPROJECTDIR in CMTPROJECTPATH
file(WRITE ${PROJECT_BINARY_DIR}/setup.sh "#!/bin/bash

filename=`readlink -f \"\${BASH_SOURCE[0]}\"`
basedir=`dirname \"\${filename}\"`

export CMTPROJECTPATH=\"\$basedir/${_CMTPROJECTPATH}\${CMTPROJECTPATH:+:\${CMTPROJECTPATH}}\"
export PYTHONPATH=\"\$basedir/${CMAKE_INSTALL_PYTHONDIR}\${PYTHONPATH:+:\${PYTHONPATH}}\"
export LD_LIBRARY_PATH=\"\$basedir/${CMAKE_INSTALL_LIBDIR}\${LD_LIBRARY_PATH:+:\${LD_LIBRARY_PATH}}\"
")
configure_file("${PROJECT_SOURCE_DIR}/${${PROJECT_NAME}_TOLOW}-config.in" "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TOLOW}-config" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/${${PROJECT_NAME}_TOLOW}-modulefile.in" "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TOLOW}-modulefile" @ONLY)
if(BUILD_CMT_PROJECT)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmt/project.cmt "project sniper\n\nuse ExternalInterface\n\nbuild_strategy with_installarea\nsetup_strategy no_root\nsetup_strategy no_config\nstructure_strategy without_version_directory\n")
  file(COPY cmt/version.cmt DESTINATION cmt)
endif()

#
# install
if(USE_SIMPLE_DIRS)
  install(FILES ${PROJECT_BINARY_DIR}/setup.sh DESTINATION .)
else()
  install(FILES ${PROJECT_BINARY_DIR}/setup.sh DESTINATION ${CMAKE_INSTALL_DATADIR})
endif()
install(PROGRAMS "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TOLOW}-config" DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES "${PROJECT_BINARY_DIR}/${${PROJECT_NAME}_TOLOW}-modulefile" DESTINATION ${CMAKE_INSTALL_DATADIR}/modulefiles RENAME ${${PROJECT_NAME}_TOLOW})
if(BUILD_CMT_PROJECT)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmt DESTINATION ${CMAKE_INSTALL_CMTPROJECTDIR})
endif()
