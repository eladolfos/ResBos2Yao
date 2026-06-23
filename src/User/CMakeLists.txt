add_library(ResbosUser)
target_sources(ResbosUser
    PRIVATE
    UserCuts.cc
    UserHistograms.cc
    )

target_link_libraries(ResbosUser PRIVATE project_options project_warnings project_addons)
