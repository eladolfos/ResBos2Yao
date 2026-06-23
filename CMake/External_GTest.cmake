ExternalProject_Add(GTest
    GIT_REPOSITORY ${GTest_git}
    GIT_TAG "master"
    PREFIX ${RESBOS_BUILD_PREFIX}
    CMAKE_ARGS
        ${COMMON_CMAKE_ARGS}
)

set(GTEST_ROOT ${RESBOS_BUILD_INSTALL_PREFIX} CACHE STRING "")

file(APPEND ${RESBOS_CONFIG_INPUT} "
#################################
# GTest
#################################
set(GTEST_ROOT \${RESBOS_ROOT})
set(RESBOS_ENABLED_GTest TRUE)
")
