## generate
string(TIMESTAMP CONFIG_DATE)
set(CONFIG_DATE ${CONFIG_DATE} CACHE INTERNAL "Date when build was made")
set(CONFIG_USER $ENV{USER} CACHE INTERNAL "Name who did the build")
configure_file("${PROJECT_SOURCE_DIR}/cmake/sniper-config.in" "${PROJECT_BINARY_DIR}/sniper-config" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/sniper-init.json.in" "${PROJECT_BINARY_DIR}/sniper-init.json" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/sniper-setup.sh.in" "${PROJECT_BINARY_DIR}/sniper-setup.sh" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/sniper-modulefile.in" "${PROJECT_BINARY_DIR}/sniper-modulefile" @ONLY)

## install
install(PROGRAMS "${PROJECT_BINARY_DIR}/sniper-config" DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES "${PROJECT_BINARY_DIR}/sniper-init.json" DESTINATION ${CMAKE_INSTALL_DATADIR}/sniper RENAME .init.json)
install(FILES "${PROJECT_BINARY_DIR}/sniper-setup.sh" DESTINATION ${CMAKE_INSTALL_DATADIR}/sniper RENAME setup.sh)
install(FILES "${PROJECT_BINARY_DIR}/sniper-modulefile" DESTINATION ${CMAKE_INSTALL_DATADIR}/sniper/modulefiles RENAME sniper)

## for cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SNiPERConfigVersion.cmake
    VERSION ${SNiPER_VERSION}
    COMPATIBILITY SameMajorVersion
    )

# - install time config and target files
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/SNiPERConfig.cmake.in
    "${PROJECT_BINARY_DIR}/SNiPERConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    PATH_VARS
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_LIBDIR
    )

# - install and export
install(FILES
    "${PROJECT_BINARY_DIR}/SNiPERConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/SNiPERConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    FILE "${PROJECT_NAME}Targets.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/"
    )
