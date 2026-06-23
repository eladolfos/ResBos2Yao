include(CMakeParseArguments)

function(get_system_library_vars prefix extension)
    set(${prefix} "lib" PARENT_SCOPE)
    if(APPLE)
        set(${extension} "dylib" PARENT_SCOPE)
    else()
        set(${extension} "so" PARENT_SCOPE)
    endif()
endfunction()

function(get_system_library_name lib_name result)
    get_system_library_vars(pre ext)
    set(${result} "${pre}${lib_name}.${ext}" PARENT_SCOPE)
endfunction()

function(format_passdowns _str _varResult)
    set(_tmpResult "")
    get_cmake_property(_vars VARIABLES)
    string(REGEX MATCHALL "(^|;)${_str}[A-Za-z0-9]*" _matchedVars "${_vars}")
    foreach( _match ${_matchedVars} )
        set( _tmpResult ${_tmpResult} "-D${_match}=${${_match}}" )
    endforeach()
    set( ${_varResult} ${_tmpResult} PARENT_SCOPE )
endfunction()

macro(add_package_dependency)
    set(options
        OPTIONAL
        EMBEDDED
        )
    set(oneValueArgs
        PACKAGE
        PACKAGE_DEPENDENCY

        # Optional args
        PACKAGE_DEPENDENCY_ALIAS
        )
    set(multiValueArgs)

    cmake_parse_arguments(MY
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
        )

    if(NOT MY_OPTIONAL AND MY_EMBEDDED)
        message(FATAL_ERROR "EMBEDDED option is ON while ${MY_PACKAGE_DEPENDENCY} is not optional \n"
            "-> ${MY_PACKAGE_DEPENDENCY} should be an optional dependency.")
    endif()

    set(${MY_PACKAGE}_WITH_${MY_PACKAGE_DEPENDENCY})
    if(RESBOS_ENABLE_${MY_PACKAGE_DEPENDENCY})
        list(APPEND ${MY_PACKAGE}_DEPENDS ${MY_PACKAGE_DEPENDENCY})
        set(${MY_PACKAGE}_WITH_${MY_PACKAGE_DEPENDENCY} ON)
    else()
        set(dependency_name ${MY_PACKAGE_DEPENDENCY})
        if(MY_PACKAGE_DEPENDENCY_ALIAS)
            set(dependency_name ${MY_PACKAGE_DEPENDENCY_ALIAS})
        endif()

        find_package(${dependency_name} QUIET)

        string(TOUPPER ${dependency_name}_FOUND uppercase_found)
        
        if(DEFINED ${dependency_name}_FOUND)
            set(dependency_found ${${dependency_name}_FOUND})
        elseif(DEFINED uppercase_found)
            set(dependency_found ${uppercase_found})
        endif()

        if(NOT dependency_found AND MY_OPTIONAL)
            if(MY_EMBEDDED)
                message(STATUS "Warning: ${MY_PACKAGE} will rely on its own internal build of ${dependency_name}.")
            else()
                message(STATUS "Warning: ${dependency_name} disabled for ${MY_PACKAGE} build")
            endif()
        elseif(NOT dependency_found AND NOT MY_OPTIONAL)
            message(FATAL_ERROR
                " ${dependency_name} is required to build ${MY_PACKAGE}.\n "
                "Either:\n "
                "- Turn on RESBOS_ENABLE_${MY_PACKAGE_DEPENDENCY}.\n "
                "- Provide the location of an external ${dependency_name}.\n"
                )
        endif()
        set(${MY_PACKAGE}_WITH_${MY_PACKAGE_DEPENDENCY} ${${dependency_name}_FOUND})
    endif()
endmacro()

function(remove_file_before)
    set(options)
    set(oneValueArgs TARGET FILE)
    set(multiValueArgs)
    cmake_parse_arguments(MY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT MY_TARGET)
        message(FATAL_ERROR "Error - Unkown target: ${MY_TARGET}")
    endif()
    if(NOT MY_FILE)
        message(FATAL_ERROR "Error - File is not valid")
    endif()

    add_custom_command(TARGET ${MY_TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove "${MY_FILE}"
        COMMENT "Removing file ${MY_FILE} before target '${MY_TARGET}'"
        )
endfunction()

function(RESBOS_external_project_force_install)
    set(options)
    set(oneValueArgs PACKAGE)
    set(multiValueArgs STEP_NAMES)
    cmake_parse_arguments(MY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    IF(NOT MY_STEP_NAMES)
        set(MY_STEP_NAMES "install")
    endif()

    ExternalProject_Add_StepTargets(${MY_PACKAGE} install)

    foreach(step_name ${MY_STEP_NAMES})
        set(stamp_file_root "${RESBOS_BINARY_DIR}/build/src/${MY_PACKAGE}-stamp")
        if(CMAKE_CONFIGURATION_TYPES)
            set(install_stamp_file "${stamp_file_root}/${CMAKE_CFG_INTDIR}/${MY_PACKAGE}-${step_name}")
        else()
            set(install_stamp_file "${stamp_file_root}//${MY_PACKAGE}-${step_name}")
        endif()

        remove_file_before(TARGET ${MY_PACKAGE}-install FILE ${install_stamp_file})
        set_property(GLOBAL APPEND PROPERTY RESBOS_INSTALL_STAMP_FILES ${install_stamp_file})
    endforeach()
endfunction()
