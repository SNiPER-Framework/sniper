# The CMakeLists for SNiPER
cmake_minimum_required(VERSION 3.12)
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

# Project and Build Options
project(SNiPER VERSION 2.0)

option(DEBUG       "Turn debug options on" OFF)
option(USE_PYTHON  "Whether to bind the project to Python" ON)
option(USE_ROOT    "Whether to build the components that depend on ROOT" ON)

# Global Settings
include(CMakePrintHelpers)
include(GNUInstallDirs)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DDARWIN)
endif()
if(WIN32)
    add_definitions(-DWIN32)
endif()

# Compiler and Linker Settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed -Wl,--no-undefined")
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
endif()

# Config for Option DEBUG
if(DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

# Config for Option USE_PYTHON
if(USE_PYTHON)
    #python
    set(PyPkgName Python3)  #Use Python3 by default
    option(USE_PYTHON2 "Use Python2 instead of Python3" OFF)
    if(USE_PYTHON2)
        set(PyPkgName Python2)
    endif()
    find_package(${PyPkgName} COMPONENTS Interpreter Development REQUIRED)
    message(STATUS "python version ${${PyPkgName}_VERSION}")
    set(PYTHON_VERSION "${${PyPkgName}_VERSION}")
    set(PYTHON_INCLUDE_DIRS "${${PyPkgName}_INCLUDE_DIRS}")
    set(PYTHON_LIBRARY "${${PyPkgName}_LIBRARY}")
    #boost.python
    set(Boost_USE_MULTITHREADED OFF)
    if(USE_PYTHON2)
        find_package(Boost 1.53 COMPONENTS python REQUIRED)
        set(BOOST_PYTHON_LIBRARY "${Boost_PYTHON_LIBRARY}")
    else()
        find_package(Boost 1.53 COMPONENTS python3 REQUIRED)
        set(BOOST_PYTHON_LIBRARY "${Boost_PYTHON3_LIBRARY}")
    endif()
    #python install dir
    if(NOT CMAKE_INSTALL_PYTHONDIR)
        set(CMAKE_INSTALL_PYTHONDIR "python")
    endif()
    #...
    add_definitions(-DHAVE_PYTHON)
    unset(PyPkgName)
endif()

# Config for Option USE_ROOT
if(USE_ROOT)
    find_package(ROOT 5.18 REQUIRED)
    message(STATUS "ROOT version ${ROOT_VERSION}")
endif()

# Build Target SniperKernel
add_subdirectory(SniperKernel)
# Build Target SniperPython
if(USE_PYTHON)
    add_subdirectory(SniperPython)
endif()
# Build Target Utilities
add_subdirectory(SniperUtil)
# Build Target Examples
add_subdirectory(Examples)
# Build Target Main Executable
add_subdirectory(src)

# Generate sniper-config
string(TIMESTAMP CONFIG_DATE)
set(CONFIG_DATE ${CONFIG_DATE} CACHE INTERNAL "Date when build was made")
set(CONFIG_USER $ENV{USER} CACHE INTERNAL "Name who did the build")
configure_file("${PROJECT_SOURCE_DIR}/cmake/sniper-config.in" "${PROJECT_BINARY_DIR}/sniper-config" @ONLY)

# Install
install(PROGRAMS "${PROJECT_BINARY_DIR}/sniper-config" DESTINATION ${CMAKE_INSTALL_BINDIR})

# Messages for DEBUG
message(STATUS "*****************")
cmake_print_variables(CMAKE_SYSTEM_NAME)
cmake_print_variables(CMAKE_CXX_FLAGS)
cmake_print_variables(CMAKE_SHARED_LINKER_FLAGS)
cmake_print_variables(USE_PYTHON PYTHON_INCLUDE_DIRS Boost_INCLUDE_DIRS PYTHON_LIBRARY BOOST_PYTHON_LIBRARY)
cmake_print_variables(USE_ROOT ROOT_INCLUDE_DIRS ROOT_Core_LIBRARY)
cmake_print_variables(CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_DATADIR CMAKE_INSTALL_PYTHONDIR)
message(STATUS "*****************")