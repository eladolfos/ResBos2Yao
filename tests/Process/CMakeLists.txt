# Load in gtest
include(FetchContent)
FetchContent_GetProperties(extern_gtest)

# Build tests for each process class
add_executable(testProcess test.cc Process_test.cc)
target_link_libraries(testProcess PRIVATE project_options project_warnings project_addons
                                          coverage_config
                                  PUBLIC gtest_main gtest gmock ResbosUtilities ResbosProcess 
                                         Resbos dl
)
add_test(testProcess testProcess)
