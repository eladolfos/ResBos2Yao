include_directories(${FFTW_INCLUDE_DIR})
include_directories(${RESBOS_SOURCE_DIR}/include)
add_library(FFTWPP "")
target_link_libraries(FFTWPP LINK_PUBLIC
    ${RESOBS_FFTWPP_LIBRARIES}
    ${FFTW_LIBRARY}
    )
target_sources(FFTWPP
    PRIVATE
    Array.cc
    Complex.cc
    convolution.cc
    fftw++.cc
    )

target_include_directories(FFTWPP PUBLIC ${RESOBS_SOURCE_DIR}/include)

install(TARGETS FFTWPP
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
