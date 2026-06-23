cmake_minimum_required(VERSION 3.9)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Policy to address @foo@ variable expansion
if(POLICY CMP0053)
    cmake_policy(SET CMP0053 NEW)
endif()

# Set the project name and basic settings
project(RESBOS CXX)
include(CMake/StandardProjectSettings.cmake)

set(RESBOS_CMAKE_DIR "${RESBOS_SOURCE_DIR}/CMake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(RESBOS_DOWNLOAD_DIR ${RESBOS_SOURCE_DIR}/Downloads CACHE PATH
    "Directory to download tarballs into.")

# Options
include(CMake/ProgramOptions.cmake)

if(USE-CPACK)
    include(CMake/Package.cmake)
endif()

include(CMake/Utils.cmake)
include(FindPkgConfig)

add_custom_target(Download)
add_custom_target(Resbos-build-install)

# Link this 'library' to set the c++ standard / compile-time options requested
# Addtionally, link to get include and external dependencies
add_library(project_options INTERFACE)
target_include_directories(project_options INTERFACE include external)

# Linnk this 'library' to use warning specifed in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# Standard compiler warnings
#include(CMake/CompilerWarnings.cmake)
#set_project_warnings(project_warnings)

# Sanitizer options if supported by compiler
include(CMake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Allow for static analysis options
include(CMake/StaticAnalyzers.cmake)

# Load optional libraries
add_library(project_addons INTERFACE)
load_options(project_addons)

if(FITTING)
    MESSAGE(STATUS "Looking for BAT Libraries")
    find_package(BAT)
    if(NOT BAT_FOUND)
        message(FATAL_ERROR "Failed to find BAT environment"
            " (use -DFITTING=OFF if you didn't want to build with Fitting functionality.")
    endif(NOT BAT_FOUND)
    add_definitions(-DFITTING)
    add_library(fitting INTERFACE)
    target_include_directories(fitting INTERFACE ${BAT_INCLUDE_DIR})
    target_link_libraries(fitting INTERFACE ${BAT_LIBRARY})
    # Require Eigen
    find_package(Eigen3 REQUIRED)
    file(COPY ${RESBOS_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif(FITTING)

# Initialize pthreads information
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Initialize LHAPDF information
add_library(lhapdf INTERFACE)
find_package(LHAPDF REQUIRED)
target_include_directories(lhapdf SYSTEM INTERFACE ${LHAPDF_INCLUDE_DIR})
target_link_libraries(lhapdf INTERFACE ${LHAPDF_LIBRARY})

# Initialize Hoppet information
add_library(hoppet INTERFACE)
find_package(Hoppet REQUIRED)
target_include_directories(hoppet SYSTEM INTERFACE ${Hoppet_INCLUDE_DIR})
target_link_libraries(hoppet INTERFACE ${Hopppet_LIBRARY} gfortran)

# Check if python extension is asked for
if(PYEXT)
    FIND_PACKAGE(SWIG REQUIRED)
    INCLUDE(UseSWIG)

    FIND_PACKAGE(Python COMPONENTS Development REQUIRED)
endif(PYEXT)

# Find backtrace libraries
add_library(backtrace INTERFACE)
find_package(Backtrace)
target_link_libraries(backtrace INTERFACE ${Backtrace_LIBRARIES})

# Logging settings
add_definitions(-DLOGURU_WITH_STREAMS=1)
add_definitions(-DLOGURU_UNSAFE_SIGNAL_HANDLER=0)

# if(NOT RESBOS_BUILD_INSTALL_PREFIX)
#     set(RESBOS_BUILD_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
#     set(RESBOS_CONFIG_OUTPUT ${RESBOS_BINARY_DIR}/ResbosConfig.cmake)
# else()
#     set(RESBOS_CONFIG_OUTPUT ${RESBOS_BUILD_INSTALL_PREFIX}/share/cmake/ResbosConfig.cmake)
# endif()

set(RESBOS_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS ${RESBOS_BUILD_DIR})
# configure_file(
#     ${RESBOS_SOURCE_DIR}/ResbosConfig-version.cmake.in
#     ${RESBOS_BUILD_DIR}/ResbosConfig-version.cmake
#     @ONLY IMMEDIATE)

# Add in location of data files to HiggsBranching.hh
configure_file(
    ${RESBOS_SOURCE_DIR}/include/ResBos/HiggsBranching.hh.in
    ${RESBOS_SOURCE_DIR}/include/ResBos/HiggsBranching.hh
    @ONLY IMMEDIATE)

# External projects are responsible for updating, so that RESBOS can find dependent projects
# set(RESBOS_CONFIG_INPUT ${RESBOS_BINARY_DIR}/ResbosConfig.cmake.in)
# set(RESBOS_BUILD_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/build)

# Using RESBOS_INSTALL_BUILD_DIR to define where RESBOS_ROOT is at configure time
# set(RESBOS_INSTALL_BUILD_DIR ${RESBOS_BUILD_INSTALL_PREFIX})
# file(WRITE ${RESBOS_CONFIG_INPUT} "
# # Configuration file for the RESBOS build
# set(RESBOS_VERSION ${RESBOS_VERSION})
# set(RESBOS_ROOT @RESBOS_INSTALL_BUILD_DIR@)
# ")

option(BUILD_SHARED_LIBS "Build shared libraries where possible." ON)
mark_as_advanced(BUILD_SHARED_LIBS)

include(CMake/CPM.cmake)
add_subdirectory(external)

include(Resbos-tarballs)
# include(ExternalProject)

# set(COMMON_CMAKE_ARGS
#     -DCMAKE_INSTALL_PREFIX:PATH=${RESBOS_BUILD_INSTALL_PREFIX}
#     -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
#     -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#     -DCMAKE_PREFIX_PATH:PATH=${RESBOS_BUILD_INSTALL_PREFIX}
#     -DCMAKE_INSTALL_LIBDIR=lib
#     )
# 
# # Setup RESBOS targets
# set_property(DIRECTORY PROPERTY EP_STEP_TARGETS download)
# foreach(source ${RESBOS_external_sources})
#     include(External_${source})
#     set(RESBOS_current_package ${source})
#     add_dependencies(Download ${source}-download)
# 
#     if(TARGET ${source}-install)
#         add_dependencies(RESBOS-build-install ${source}-install)
#         add_custom_command(TARGET RESBOS-build-install
#             COMMAND ${CMAKE_COMMAND} --build ${RESBOS_BINARY_DIR} --target ${source}-install
#             )
#         set_target_properties(${source}-install PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD True)
#         set_target_properties(${source}-install PROPERTIES EXCLUDE_FROM_ALL True)
#     endif()
# 
#     set(select_version_key ${source}_SELECT_VERSION)
#     set(select_version_val ${${select_version_key}})
#     if(select_version_val)
#         file(APPEND ${RESBOS_CONFIG_INPUT} "set(${select_version_key} ${select_version_val})")
#     endif()
# endforeach()
# 
# get_property(RESBOS_INSTALL_STAMP_FILES GLOBAL PROPERTY RESBOS_INSTALL_STAMP_FILES)
# foreach(install_stamp_file ${RESBOS_INSTALL_STAMP_FILES})
#     remove_file_before(TARGET RESBOS-build-install FILE ${install_stamp_file})
# endforeach()

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib64")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib64" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib64")
endif()

# set(RESBOS_ENABLE_ALL_PACKAGES FALSE CACHE BOOL "" FORCE)

# configure_file(${RESBOS_CONFIG_INPUT} ${RESBOS_CONFIG_OUTPUT} @ONLY)

# Create needed directories for storing ConvGrids and Grids
file(MAKE_DIRECTORY ${RESBOS_BUILD_DIR}/ConvGrids)
file(MAKE_DIRECTORY ${RESBOS_BUILD_DIR}/Grids)

# Copy over default resbos.config file
if(NOT EXISTS ${RESBOS_BUILD_DIR}/resbos.config)
    configure_file(resbos.config resbos.config COPYONLY IMMEDIATE)
endif()

# If fitting, copy over default fitting.config, parameters.yml, experiments.yml,
# and the default ptData.txt file
if(FITTING)
    if(NOT EXISTS ${RESBOS_BUILD_DIR}/fitting.config)
        configure_file(fitting.config fitting.config COPYONLY IMMEDIATE)
    endif()
    if(NOT EXISTS ${RESBOS_BUILD_DIR}/parameters.yml)
        configure_file(parameters.yml parameters.yml COPYONLY IMMEDIATE)
    endif()
    if(NOT EXISTS ${RESBOS_BUILD_DIR}/experiments.yml)
        configure_file(experiments.yml experiments.yml COPYONLY IMMEDIATE)
    endif()
    if(NOT EXISTS ${RESBOS_BUILD_DIR}/ptData.txt)
        configure_file(ptData.txt ptData.txt COPYONLY IMMEDIATE)
    endif()
endif()


add_compile_options(-ftemplate-depth=1600)
check_cxx_compiler_flag(-fconstexpr-ops-limit=100000 has_constexpr_limit)
# if(${has_constexpr_limit})
add_compile_options(-fconstexpr-ops-limit=1000000000)
# endif()
add_subdirectory(src)
#add_subdirectory(scripts)
if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    MESSAGE(STATUS "Enabling tests")
    enable_testing()
    add_library(coverage_config INTERFACE)
    target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
    target_link_libraries(coverage_config INTERFACE --coverage)
    add_subdirectory(tests)
endif()
