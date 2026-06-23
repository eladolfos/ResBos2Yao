# Load in gtest
include(FetchContent)
FetchContent_GetProperties(extern_gtest)

# Build tests for Utilities
add_executable(testUtilities 
    test.cc 
    BaseIntegrator_test.cc
    Settings_test.cc
    FourVector_test.cc 
    ThreeVector_test.cc 
    Electroweak_test.cc 
    Grid2D_test.cc 
    Grid3D_test.cc
    OgataQuad_test.cc
    PhaseSpace_test.cc 
    Random_test.cc 
    ThreadPool_test.cc 
    Utility_test.cc
    Vegas_test.cc 
    LHEF_test.cc
)
target_link_libraries(testUtilities PRIVATE project_options project_warnings project_addons
                                            coverage_config
                                    PUBLIC gtest_main gtest gmock
                                           ResbosUtilities ResbosUser dl
)
add_test(testUtilities testUtilities)

