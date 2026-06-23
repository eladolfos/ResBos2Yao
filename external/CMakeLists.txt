include(FetchContent)

# fmt
CPMFindPackage(
    NAME fmt
    # Use this commit which eliminates variable shadowing but is a stable point
    GIT_TAG 63479c8519c7fa3571707ec7f40b87438b9b9c51
    GITHUB_REPOSITORY fmtlib/fmt
)
add_library(fmt::fmt ALIAS fmt)

CPMAddPackage(
    NAME yaml-cpp
    GITHUB_REPOSITORY jbeder/yaml-cpp
    GIT_TAG 4edff1fa5dbfca16fc72d89870841bee89f8ef89
    OPTIONS
        "YAML_CPP_BUILD_TESTS OFF"
        "YAML_CPP_BUILD_CONTRIB OFF"
        "YAML_CPP_BUILD_TOOLS OFF"
)

# gtest
FetchContent_Declare(
    extern_gtest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "release-1.10.0"
)

FetchContent_GetProperties(extern_gtest)
if(NOT extern_gtest_POPULATED)
    FetchContent_Populate(extern_gtest)
    add_subdirectory(
        ${extern_gtest_SOURCE_DIR}
        ${extern_gtest_BINARY_DIR}
        EXCLUDE_FROM_ALL
    )
endif()
