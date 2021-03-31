
# Define project
#project(modules)

#
file(GLOB dirlist *)
foreach(file ${dirlist})
  if(IS_DIRECTORY ${file})
    add_subdirectory(${file})
  endif()
endforeach()
#set(modlist ${modlist} PARENT_SCOPE)
