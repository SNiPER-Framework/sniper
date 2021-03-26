# configure
configure_file("${CMAKE_CURRENT_LIST_DIR}/SniperConfig.h.in" "${PROJECT_BINARY_DIR}/SniperConfig.h")

# build
add_executable(sniper.exe sniper.cc)
target_include_directories(sniper.exe PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(sniper.exe dl)

# install
install(TARGETS sniper.exe DESTINATION ${CMAKE_INSTALL_BINDIR})