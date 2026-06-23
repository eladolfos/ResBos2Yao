add_library(ResbosProcess)
target_sources(ResbosProcess
    PRIVATE
    Process.cc
    ProcessFactory.cc
    A0.cc
    DrellYan.cc
    Higgs.cc
    Wpm.cc
    Z0.cc
    )

target_link_libraries(ResbosProcess PRIVATE project_options project_warnings project_addons
                                    PUBLIC ResbosUtilities)
