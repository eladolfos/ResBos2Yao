# Load in gtest
include(FetchContent)
FetchContent_GetProperties(extern_gtest)

# Build tests for calculations
add_executable(testCalculation 
    test.cc 
    Asymptotic_test.cc
    DeltaSigma_test.cc
    Resummation_test.cc
)
target_link_libraries(testCalculation PRIVATE project_options project_warnings project_addons
                                              coverage_config
                                      PUBLIC gtest_main gtest gmock ResbosUtilities                 
                                             ResbosCalculation Resbos dl
)
add_test(testCalculation testCalculation)

