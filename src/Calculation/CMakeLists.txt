add_library(ResbosCalculation)
target_sources(ResbosCalculation
    PRIVATE
    Calculation.cc
    CalculationFactory.cc
    Asymptotic.cc
    DeltaSigma.cc
    Perturbative.cc
    Resummation.cc
    # Total.cc
    WmA.cc
    # YPiece.cc
    )

target_link_libraries(ResbosCalculation PRIVATE project_options project_warnings project_addons
                                        PUBLIC ResbosUtilities)
