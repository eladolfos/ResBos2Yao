# Let CMake monitor git status to ease bug reporting
# include(GetGitRevisionDescription)
# if(GIT_REFSPEC STREQUAL "GITDIR-NOTFOUND")
#     add_definitions(-DNO_GIT)
# else(GIT_REFSPEC STREQUAL "GITDIR-NOTFOUND")
#     get_git_head_revision(GIT_REFSPEC GIT_SHA1)
#     git_local_changes_file(GIT_CHANGED_FILES)
#     configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cc.in" "${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cc" @ONLY)
# endif(GIT_REFSPEC STREQUAL "GITDIR-NOTFOUND")

if(USE-GZIP)
    add_subdirectory(gzstream)
endif(USE-GZIP)

add_subdirectory(Utilities)
add_subdirectory(Beam)
add_subdirectory(Calculation)
add_subdirectory(Process)
add_subdirectory(User)

add_library(Resbos)
target_sources(Resbos
    PRIVATE
    ResBos.cc
    )
target_link_libraries(Resbos PRIVATE project_options project_warnings project_addons
                             PUBLIC ResbosUtilities ResbosBeam ResbosCalculation ResbosProcess
                                    ResbosUser fmt::fmt dl)


add_library(MCFMInterface)
target_sources(MCFMInterface
    PRIVATE
    MCFMInterface.cc
    )

target_link_libraries(MCFMInterface PRIVATE project_options project_warnings project_addons
                                    PUBLIC Resbos)

add_executable(resbos main.cc) #GitSHA1.cc)
target_link_libraries(resbos PRIVATE project_options project_warnings project_addons
                             PUBLIC Resbos)

install(TARGETS ResbosUtilities ResbosBeam ResbosCalculation ResbosProcess
                ResbosUser Resbos MCFMInterface resbos fmt yaml-cpp
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(FITTING)
    add_subdirectory(NonPertFit)
endif(FITTING)

if(PYEXT)
    add_subdirectory(PythonExt)
endif(PYEXT)

#include_directories(${Resbos_SOURCE_DIR}/include)
#add_library(resbosfitlib "")
#set_target_properties(resbosfitlib PROPERTIES DEFINE_SYMBOL FITTING)
#include_directories(${LHAPDF_INCLUDE_DIR})
#include_directories(${Hoppet_INCLUDE_DIR})
#include_directories(${ROOT_INCLUDE_DIR})
#include_directories(${YODA_INCLUDE_DIR})
#target_link_libraries(resbosfitlib ${RESBOS_LIBRARIES} ${LHAPDF_LIBRARY} ${Hoppet_LIBRARY} gfortran)
#target_sources(resbosfitlib
#    PRIVATE
#    )
#target_include_directories(resbosfitlib PUBLIC ${Resbos_SOURCE_DIR}/include)
