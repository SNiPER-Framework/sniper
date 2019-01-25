#
# - Try to find the CMT build system (http://www.cmtsite.net/).
#
# The module defines the following variables:
#   CMT_FOUND          - true if CMT was found
#   CMT_EXECUTABLE     - path to command line tool
#   CMT_VERSION_STRING - the version of CMT found
#
# Working CMT system can be used to find packages with another software,
# that were configured to be available through the CMT.
#
#   cmt_find_package(name [major[.minor[.patch[.tweak]]]] [EXACT]
#               [QUIET] [REQUIRED] [[COMPONENTS] [components...]]
#               [PATHS path1 [paths...]] )
#
# When the 'REQUIRED' argument was set, it will fail with an error
# if package could not be found
#
# When the 'QUIET' argument is set, no status messages will be printed.
#
# It searches for the first package with version greater of (if no 'EXACT')
# or equal to the provided one.  Package version is taken according to CMT
# (from cmt/version.cmt file).  Search path is defined as (<name>_CMT_DIR,
# caller hints by 'PATHS', CMT_PREFIX_PATH and "Externals").  If the package
# is found and its ROOT_DIR is located, the code tries to call a regular
# find_package() with this ROOT_DIR as a hint.
#
# It sets the following variables:
#   <name>_FOUND        - true if package was found
#   <name>_VERSION      - package version according to CMT
#   <name>_ROOT_DIR     - package root directory
#   <name>_INCLUDE_DIRS - path to search for header files
#   <name>_DEFINITIONS  - the '-D' preprocessor flags
#   <name>_CPPFLAGS     - other compiler flags
#   <name>_LIBRARY_DIRS - path to search for libraries
#   <name>_LIBRARIES    - full list of liraries from '_linkopts'
#   <name>_LDFLAGS      - other linker flags
# All values are derived from CMT variables, no direct interaction with the
# software is performed.

#=============================================================================
# Copyright (C) 2018 Vladimir Belov, <belov@itep.ru>
#
# Distributed under the OSI-approved BSD 3-Clause License.
# See https://cmake.org/licensing for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

#set(CMT_VERSION 1)
set(CMT_PREFIX_PATH "" CACHE PATH "Path to a directory with CMT packages.")
find_program(CMT_EXECUTABLE NAMES cmt DOC "CMT main executable")
mark_as_advanced(CMT_EXECUTABLE)

# convert version from CMT style to regular with dots
function(_cmt_convert_version _name _vers)
  set(out ${_vers})
  string(REPLACE "v" ""  out "${out}")
  string(REPLACE "r" "." out "${out}")
  string(REPLACE "p" "." out "${out}")
  set(${_name} ${out} PARENT_SCOPE)
endfunction()

if (CMT_EXECUTABLE)
  execute_process(
    COMMAND ${CMT_EXECUTABLE} version
    OUTPUT_VARIABLE out
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  _cmt_convert_version(CMT_VERSION_STRING ${out})
  unset(out)
endif()

# handle the QUIET and REQUIRED arguments and set CMT_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CMT REQUIRED_VARS CMT_EXECUTABLE VERSION_VAR CMT_VERSION_STRING)
include(FindPackageMessage)
# package loaded

#
# Now stuff related to packages finding
#

# List all packages reachable by CMT
macro(_cmt_list_packages _name)

  if(NOT cmt_wrap)
    _cmt_create_wrap()
  endif()

  execute_process(
    COMMAND ${cmt_wrap} show packages
    RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
    string(REPLACE "\n" ";" _clp_lines ${out})
    foreach(_clp_line ${_clp_lines})
      if(${_clp_line} MATCHES "^${NAME} (.+) ")
        string(LENGTH ${CMAKE_MATCH_0} _clp_tmp)
        string(SUBSTRING ${_clp_line} ${_clp_tmp} -1 _clp_tmp)
        list(APPEND ${_name}_CMT_DIRS ${_clp_tmp})
      endif()
    endforeach()
    unset(_clp_lines)
    unset(_clp_line)
    unset(_clp_tmp)
  endif()
endmacro()

# Check if package is available and get its version and root
macro(_cmt_check_package NAME VERSION DIR)

  if(NOT cmt_wrap)
    _cmt_create_wrap()
  endif()

  set(cmt_dir "CMakeFiles/cmt_test_${NAME}/cmt")
  # Creating new test package
  execute_process(
    COMMAND ${cmt_wrap} create cmt_test_${NAME} v0 CMakeFiles
    RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  # and adding our dependency to it
  file(APPEND ${PROJECT_BINARY_DIR}/${cmt_dir}/requirements
    "use ${NAME} ${VERSION} ${DIR}\n")

  # can CMT find it ?
  execute_process(
    COMMAND ${cmt_wrap} check configuration
    WORKING_DIRECTORY ${cmt_dir}
    RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if("${err}" STREQUAL "") # check conf

    # get package version
    # here we assume that version by CMT is correct
    string(TOUPPER "${NAME}" _name_up)
    execute_process(
      COMMAND ${cmt_wrap} show macro_value ${_name_up}VERSION
      WORKING_DIRECTORY ${cmt_dir}
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
      _cmt_convert_version(${NAME}_VERSION ${out})
    endif()

    # get package root
    execute_process(
      COMMAND ${cmt_wrap} show macro_value ${NAME}_root
      WORKING_DIRECTORY ${cmt_dir}
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
      set(${NAME}_ROOT ${out})
    endif()

  endif() # check conf

  # let's clean out
  get_filename_component(cmt_dir ${cmt_dir} DIRECTORY)
  file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/${cmt_dir})
  unset(cmt_dir)

endmacro()

# Main work function.
function(CMT_FIND_PACKAGE NAME)

  # Handle options
  set(switch_opts EXACT QUIET REQUIRED)
  set(one_val_opts )
  set(multi_val_opts COMPONENTS PATHS)
  if(NOT "${ARGN}" STREQUAL "")
    set(all_opts ${switch_opts} ${one_val_opts} ${multi_val_opts})
    list(GET ARGN 0 first_arg)
    list(FIND all_opts ${first_arg} index)
    if(${index} EQUAL -1)
      # first argument isn't a keyword, so it's version
      set(VERSION ${first_arg})
      list(REMOVE_AT ARGN 0)
    endif()
  endif()
  cmake_parse_arguments(_cfp "${switch_opts}" "${one_val_opts}" "${multi_val_opts}" ${ARGN})
  if(_cfp_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keyword given to CMT_FIND_PACKAGE(): \"${_cfp_UNPARSED_ARGUMENTS}\"")
  endif()
  #cmake_print_variables(NAME VERSION _cfp_EXACT _cfp_QUIET _cfp_REQUIRED _cfp_COMPONENTS _cfp_PATHS)

  if(NOT CMT_FOUND)
    message(SEND_ERROR "Can't find 'cmt' executable. No packages from CMT system can be used.")
    return()
  endif()

  _cmt_create_wrap()

  if( NOT ${NAME}_CMT_DIR OR
      NOT EXISTS "${${NAME}_CMT_DIR}/cmt/requirements")

    # we don't have cached value, so let's go find something
    set(${NAME}_CMT_DIRS)
    set(${NAME}_CMT_DIR)
    list(APPEND ${NAME}_CMT_DIRS ${_cfp_PATHS})
    list(APPEND ${NAME}_CMT_DIRS ${CMT_PREFIX_PATH})
    list(APPEND ${NAME}_CMT_DIRS Externals)
    _cmt_list_packages(${NAME})
    list(REMOVE_DUPLICATES ${NAME}_CMT_DIRS)
    foreach(dir ${${NAME}_CMT_DIRS})
      _cmt_check_package(${NAME} * "${dir}")
      if( "${VERSION}" VERSION_LESS "${${NAME}_VERSION}" AND NOT _cfp_EXACT OR
          "${VERSION}" VERSION_EQUAL "${${NAME}_VERSION}" OR
          "${VERSION}" STREQUAL "")
        set(${NAME}_CMT_DIR ${${NAME}_ROOT} CACHE PATH "Path to a directory with CMT package of ${NAME}.")
        break()
      endif()
    endforeach()
  endif()

  if(${NAME}_CMT_DIR)

    get_filename_component(pkg_dir ${${NAME}_CMT_DIR} DIRECTORY)
    # yes, we have it
    unset(${NAME}_FOUND)
    unset(${NAME}_VERSION CACHE)
    unset(${NAME}_ROOT_DIR CACHE)
    unset(${NAME}_INCLUDE_DIRS CACHE)
    unset(${NAME}_DEFINITIONS CACHE)
    unset(${NAME}_CPPFLAGS CACHE)
    unset(${NAME}_LIBRARY_DIRS CACHE)
    unset(${NAME}_LIBRARIES CACHE)
    unset(${NAME}_LDFLAGS CACHE)

    # Creating new test package
    execute_process(
      COMMAND ${cmt_wrap} create cmt_test_${NAME} v0 CMakeFiles
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(cmt_dir "CMakeFiles/cmt_test_${NAME}/cmt")
    # and adding our dependency to it
    file(APPEND ${PROJECT_BINARY_DIR}/${cmt_dir}/requirements
      "use ${NAME} * ${pkg_dir}\n")

    # get package version
    # here we assume that CMT 'version' is correct
    string(TOUPPER "${NAME}" ${NAME}_TOUPP)
    execute_process(
      COMMAND ${cmt_wrap} show macro_value ${${NAME}_TOUPP}VERSION
      WORKING_DIRECTORY ${cmt_dir}
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
      _cmt_convert_version(${NAME}_VERSION ${out})
    endif()

    # include directories
    execute_process(
      COMMAND ${cmt_wrap} show include_dirs
      WORKING_DIRECTORY ${cmt_dir}
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
      # have something
      separate_arguments(out_list UNIX_COMMAND ${out})
      foreach(item ${out_list})
        if(NOT "${item}" STREQUAL "$(src)")
          list(APPEND ${NAME}_INCLUDE_DIRS ${item})
        endif()
      endforeach()
    endif()

    # compile flags
    execute_process(
      COMMAND ${cmt_wrap} show macro_value ${NAME}_cppflags
      WORKING_DIRECTORY ${cmt_dir}
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
      # have something
      separate_arguments(out_list UNIX_COMMAND ${out})
      foreach(item ${out_list})
        if("${item}" MATCHES "^-I(.+)")
          list(APPEND ${NAME}_INCLUDE_DIRS ${CMAKE_MATCH_1})
        elseif("${item}" MATCHES "^-D")
          list(APPEND ${NAME}_DEFINITIONS ${item})
        else()
          list(APPEND ${NAME}_CPPFLAGS ${item})
        endif()
      endforeach()
    endif()

    # link flags
    execute_process(
      COMMAND ${cmt_wrap} show macro_value ${NAME}_linkopts
      WORKING_DIRECTORY ${cmt_dir}
      RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_VARIABLE err
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if("${res}" EQUAL 0 AND NOT "${out}" STREQUAL "")
      # have something
      separate_arguments(out_list UNIX_COMMAND ${out})
      foreach(item ${out_list})
        if("${item}" MATCHES "^-L(.+)")
          list(APPEND ${NAME}_LIBRARY_DIRS ${CMAKE_MATCH_1})
        elseif("${item}" MATCHES "^-l(.+)")
          list(APPEND libs ${CMAKE_MATCH_1})
        elseif("${item}" MATCHES "^-")
          list(APPEND ${NAME}_LDFLAGS ${item})
        else()
          list(APPEND ${NAME}_LIBRARIES ${item})
        endif()
      endforeach()
      # need to expand short library names to a full paths
      foreach(item ${libs})
        set(file)
        find_library(file NAMES ${item} PATHS ${${NAME}_LIBRARY_DIRS})
        if(file)
          list(APPEND ${NAME}_LIBRARIES ${file})
          get_filename_component(dir ${file} DIRECTORY)
          list(APPEND ${NAME}_LIBRARY_DIRS ${dir})
        else()
          message(WARNING "CMT use ${NAME}: Can't find -l${item}")
        endif()
        unset(file CACHE)
      endforeach()
    endif()
    if(${NAME}_LIBRARY_DIRS)
      list(REMOVE_DUPLICATES ${NAME}_LIBRARY_DIRS)
    endif()

    # let's find package directory
    # guess it from library path, assuming <pkg_dir>/lib/lib<name> structure
    set(dir)
    string(TOUPPER "${NAME}" _name_up)
    string(TOLOWER "${NAME}" _name_low)
    foreach(item ${${NAME}_LIBRARIES})
      if(NOT dir)
        get_filename_component(dir ${item} DIRECTORY)
        if(${dir} MATCHES "/(lib|lib64)(/(${NAME}|${_name_low}|${_name_up})(-[0-9.]+)?)?$")
          string(REPLACE ${CMAKE_MATCH_0} "" dir ${dir})
          set(${NAME}_ROOT_DIR ${dir})
          if("${${NAME}_INCLUDE_DIRS}" STREQUAL "")
            # if include_dirs is empty
            list(APPEND ${NAME}_INCLUDE_DIRS ${dir}/include)
          endif()
        else()
          set(${NAME}_ROOT_DIR ${dir})
          if("${${NAME}_INCLUDE_DIRS}" STREQUAL "")
            # if include_dirs is empty
            list(APPEND ${NAME}_INCLUDE_DIRS ${dir})
          endif()
        endif()
      endif()
    endforeach()
    if(${NAME}_INCLUDE_DIRS)
      list(REMOVE_DUPLICATES ${NAME}_INCLUDE_DIRS)
    endif()

    if(${NAME}_ROOT_DIR)
      # maybe package is modern enough
      find_package(${NAME} QUIET HINTS ${${NAME}_ROOT_DIR} NO_DEFAULT_PATH)
      if(${NAME}_FOUND)
        return()
      endif()
    endif()

    # this is the end, wrap-up
    set(${NAME}_FIND_QUIETLY ${_cfp_QUIET})
    find_package_handle_standard_args(${NAME} FOUND_VAR ${NAME}_FOUND REQUIRED_VARS ${NAME}_INCLUDE_DIRS ${NAME}_LIBRARIES VERSION_VAR ${NAME}_VERSION)
    if(${${NAME}_FOUND})
      set(${NAME}_FOUND ${${NAME}_FOUND} PARENT_SCOPE)
      set(${NAME}_VERSION ${${NAME}_VERSION} PARENT_SCOPE)
      set(${NAME}_ROOT_DIR ${${NAME}_ROOT_DIR} PARENT_SCOPE)
      set(${NAME}_INCLUDE_DIRS ${${NAME}_INCLUDE_DIRS} PARENT_SCOPE)
      set(${NAME}_DEFINITIONS ${${NAME}_DEFINITIONS} PARENT_SCOPE)
      set(${NAME}_CPPFLAGS ${${NAME}_CPPFLAGS} PARENT_SCOPE)
      set(${NAME}_LIBRARY_DIRS ${${NAME}_LIBRARY_DIRS} PARENT_SCOPE)
      set(${NAME}_LIBRARIES ${${NAME}_LIBRARIES} PARENT_SCOPE)
      set(${NAME}_LDFLAGS ${${NAME}_LDFLAGS} PARENT_SCOPE)
    endif()

  else() # check conf
    if(_cfp_EXACT)
      message(WARNING "Could NOT find ${NAME}\n(Required is exact version \"${VERSION}\")")
    else()
      message(WARNING "Could NOT find ${NAME}\n(Required is at least version \"${VERSION}\")")
    endif()
  endif() # check conf

  # let's clean out
  get_filename_component(cmt_dir ${cmt_dir} DIRECTORY)
  file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/${cmt_dir})
  unset(cmt_dir)
  _cmt_remove_wrap()

endfunction()

#
# internal functions
#
function(_cmt_create_wrap)
  #
  # Stupid CMT stupidly uses stupid $PWD instead of getcwd() to determine
  # the current directory. This means 'WORKING_DIRECTORY' doesn't work for it.
  # Let's help this poor thing to find the way.
  #
  # CMake allows to set permissions only for copy.
  # To another directory.
  # **** !
  #
  set(cmt_wrap "${PROJECT_BINARY_DIR}/CMakeFiles/CMakeTmp/cmt_wrap.sh")
  file(WRITE ${cmt_wrap} "#!/bin/sh\n\n${CMT_EXECUTABLE} \"$@\"\n")
  file(COPY ${cmt_wrap}
    DESTINATION ${PROJECT_BINARY_DIR}/CMakeFiles
    FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ)
  file(REMOVE ${cmt_wrap})
  set(cmt_wrap "${PROJECT_BINARY_DIR}/CMakeFiles/cmt_wrap.sh" PARENT_SCOPE)
  # Now we have a wrapper to deal with this crap.
endfunction()

function(_cmt_remove_wrap)
  file(REMOVE ${cmt_wrap})
endfunction()
