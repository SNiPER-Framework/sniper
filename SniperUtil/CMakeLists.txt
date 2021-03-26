
# Define project
#project(modules)

#
file(GLOB dirlist *)
foreach(file ${dirlist})
  if(IS_DIRECTORY ${file})
    if (NOT USE_ROOT AND ${file} MATCHES "RootWriter" )
      continue()
    endif()
    add_subdirectory(${file})
  endif()
endforeach()
#set(modlist ${modlist} PARENT_SCOPE)
