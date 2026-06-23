# GTest
if(CMAKE_BUILD_TYPE MATCHES DEBUG) 
    set(GTest_git "https://github.com/google/googletest.git")
    list(APPEND RESBOS_external_sources GTest)
endif()

set(RESBOS_files)
foreach(source ${RESBOS_external_sources})
    option(RESBOS_ENABLE_${source} "Include ${source} version ${${source}_version}")
    set(${source}_file ${${source}_url})
endforeach()
