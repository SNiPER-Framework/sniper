
# Define project
#project(modules)

function(install_and_test package tests_env)
  math(EXPR max_i "${ARGC} - 1")
  foreach(i RANGE 2 ${max_i})
    list(GET ARGV ${i} dest)
    set(script ${CMAKE_CURRENT_LIST_DIR}/${dest})
    if(EXISTS ${script})
      install(FILES ${dest} DESTINATION ${CMAKE_INSTALL_DATADIR}/SniperExamples)
      set(test ${package}/${dest})
      #can not test python scripts due to ENVIRONMENT PYTHONPATH
      add_test(NAME ${test} COMMAND sniper.exe ${script})
      set_tests_properties(${test} PROPERTIES
        ENVIRONMENT ${tests_env}
        FAIL_REGULAR_EXPRESSION "NotFound;Failed"
        PASS_REGULAR_EXPRESSION "events processed 5.* Terminated Successfully"
      )
    else()
      message(FATAL_ERROR "can not find ${script}")
    endif()
  endforeach()
endfunction()

#
file(GLOB dirlist *)
foreach(file ${dirlist})
  if(IS_DIRECTORY ${file})
    add_subdirectory(${file})
  endif()
endforeach()
set(modlist ${modlist} PARENT_SCOPE)
