# Load in gtest
include(FetchContent)
FetchContent_GetProperties(extern_gtest)

# Build tests for calculations
add_executable(IntegrationTest test.cc 
    BeamCalculation.cc
    BeamUtility.cc
    ProcessCalculation.cc
    ResBosIntegration.cc
)
target_link_libraries(IntegrationTest PRIVATE project_options project_warnings project_addons
                                              coverage_config
                                      PUBLIC gtest_main gtest gmock ResbosUtilities ResbosBeam
                                             ResbosProcess ResbosUser ResbosCalculation Resbos dl
)
target_compile_options(IntegrationTest PUBLIC -O3)
add_test(IntegrationTest IntegrationTest)

