add_library(ResbosBeam)

target_sources(ResbosBeam
    PRIVATE
    Beam.cc
    Convolution.cc
    CT14Pdf.f
    HoppetInterface.cc
    Pdf.cc
    )
target_link_libraries(ResbosBeam PRIVATE project_options project_warnings project_addons
                                 PUBLIC ResbosUtilities lhapdf hoppet ${Hoppet_LIBRARY})

install(TARGETS ResbosBeam
        EXPORT ResbosBeam-export
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
