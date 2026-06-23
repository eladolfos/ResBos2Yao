add_executable(NonPertFit)
find_package(OpenMP)
target_sources(NonPertFit
    PRIVATE
    ../ResBos.cc
    NonPertFit.cc
    FitNonPert.cc
    Experiment.cc
    Dataset.cc
    )
target_include_directories(NonPertFit PUBLIC ${yaml-cpp_INCLUDE_DIRS})
target_link_libraries(NonPertFit PRIVATE project_options project_warnings project_addons fitting
                                 PUBLIC ResbosBeam ResbosCalculation ResbosProcess ResbosUtilities
                                        ResbosUser Threads::Threads OpenMP::OpenMP_CXX yaml-cpp
                                        Eigen3::Eigen)

install(TARGETS NonPertFit
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

add_executable(GetFixed)
target_sources(GetFixed
    PRIVATE
    ../ResBos.cc
    GetFixed.cc
    FixedTarget.cc
    )
target_include_directories(GetFixed PUBLIC)
target_link_libraries(GetFixed PRIVATE project_options project_warnings project_addons fitting
                                 PUBLIC ResbosBeam ResbosCalculation ResbosProcess ResbosUtilities
                                        ResbosUser Threads::Threads OpenMP::OpenMP_CXX
                                        Eigen3::Eigen)

install(TARGETS GetFixed
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)


    #find_package(SWIG REQUIRED)
    #include(UseSWIG)
    #
    #find_package(PythonInterp REQUIRED)
    #find_package(PythonLibs REQUIRED)
    #
    #add_library(ResbosFit)
    #target_link_libraries(ResbosFit ${RESBOS_FITTING_LIBRARIES} ${LHAPDF_LIBRARY} ${Hoppet_LIBRARY} ${BAT_LIBRARY} ${ROOT_LIBRARIES} ResbosBeam ResbosCalculation ResbosProcess ResbosUtilities ResbosUser gzstream Threads::Threads OpenMP::OpenMP_CXX)
    #target_sources(ResbosFit
    #    PRIVATE
    #    ../ResBos.cc
    #    NonPertFit.cc
    #    FitNonPert.cc
    #    Experiment.cc
    #    )
    #
    #target_include_directories(ResbosFit PUBLIC ${RESBOS_SOURCE_DIR}/include)
    #
    #include_directories(.)
    #set_property(SOURCE FitNonPert.i PROPERTY CPLUSPLUS ON)
    #swig_add_library(pyResbosFit
    #    TYPE SHARED
    #    LANGUAGE python
    #    SOURCES FitNonPert.i)
    #target_include_directories(${SWIG_MODULE_pyResbosFit_REAL_NAME} PRIVATE
    #    ${PYTHON_INCLUDE_DIRS})
    #target_link_libraries(${SWIG_MODULE_pyResbosFit_REAL_NAME} PRIVATE
    #    ResbosFit ${RESBOS_FITTING_LIBRARIES} ${LHAPDF_LIBRARY} ${Hoppet_LIBRARY} ${BAT_LIBRARY} ${ROOT_LIBRARIES} ResbosBeam ResbosCalculation ResbosProcess ResbosUtilities ResbosUser gzstream Threads::Threads OpenMP::OpenMP_CXX ${PYTHON_LIBRARIES})

    #install(TARGETS ResbosFit
    #        ARCHIVE DESTINATION lib
    #        LIBRARY DESTINATION lib
    #        RUNTIME DESTINATION bin)
