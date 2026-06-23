# Load in gtest
include(FetchContent)
FetchContent_GetProperties(extern_gtest)

# Build tests for the Beam functions
add_executable(testBeam 
    test.cc 
    Pdf_test.cc 
    HoppetInterface_test.cc 
    Beam_test.cc 
    Convolution_test.cc
)
target_link_libraries(testBeam PRIVATE project_options project_warnings project_addons
                                       coverage_config
                               PUBLIC gtest_main gtest gmock ResbosUtilities ResbosBeam
                                      Resbos dl
)
add_test(testBeam testBeam)
