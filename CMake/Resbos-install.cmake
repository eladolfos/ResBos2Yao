install(DIRECTORY ${RESBOS_BUILD_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX})

set(RESBOS_INSTALL_BUILD_DIR ${CMAKE_INSTALL_PREFIX})
set(RESBOS_CONFIG_OUTPUT ${CMAKE_INSTALL_PREFIX}/lib/cmake/Resbos/ResbosConfig.cmake)

configure_file(
    ${RESBOS_CONFIG_INPUT} ${RESBOS_BUILD_PREFIX}/config/exort/ResbosConfig_install.cmake
    @ONLY
    )

install(
    FILES
    ${RESBOS_BUILD_PREFIX}/config/export/ResbosConfig_install.cmake
    DESTINATION
    ${CMAKE_INSTALL_PREFIX}/lib/cmake/Resbos/
    RENAME
    ResbosConfig.cmake
    )

install(
    FILES
    ${RESBOS_BUILD_DIR}/ResbosConfig-version.cmake
    DESTINATION
    ${CMAKE_INSTALL_PREFIX}/lib/cmake/Resbos/
    )
