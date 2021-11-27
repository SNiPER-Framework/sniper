# build
add_executable(sniper.exe sniper.cc)
target_include_directories(sniper.exe PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(sniper.exe PRIVATE dl)

# install
install(TARGETS sniper.exe DESTINATION ${CMAKE_INSTALL_BINDIR})
