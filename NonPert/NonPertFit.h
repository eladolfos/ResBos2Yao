// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#ifndef __BAT__NONPERTFIT__H
#define __BAT__NONPERTFIT__H

#include <BAT/BCModel.h>

#include <string>
#include <vector>

// Include stuff needed for calculation
#include "NonPertFit.h"
#include "ResBos/Asymptotic.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/Grid2D.hh"
#include "ResBos/GridIO.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"
#include "ResBos/PhaseSpace.hh"
#include "ResBos/ResBos2.hh"
#include "ResBos/Resummation.hh"
#include "ResBos/ThreadPool.hh"
#include "ResBos/Total.hh"
#include "ResBos/Utility.hh"


// This is a NonPertFit header file.
// Model source code is located in file NonPert/NonPertFit.cxx

// ---------------------------------------------------------
class NonPertFit : public BCModel
{

public:

    // Constructor
    NonPertFit(const std::string& name);

    // Destructor
    ~NonPertFit();

    // Overload LogLikelihood to implement model
    double LogLikelihood(const std::vector<double>& pars);

    // Overload LogAprioriProbability if not using built-in 1D priors
    // double LogAPrioriProbability(const std::vector<double> & pars);

    // Overload CalculateObservables if using observables
    // void CalculateObservables(const std::vector<double> & pars);
    
    // ResBos calculation tools
    void InitResBos();

    // Tools for calculating LogLikelihood
    double Simpson(const int& nx, const double& dx, const std::vector<double>& f, double& err);
    double GetValueDSigmaE288(const int& type, const double& Qmin, const double& Qmax, const double& pT);

private:
    Process::Process *process;
    ResBos::Resummation *resbos;
    Utility::PDF* pdf;
    Utility::Hoppet* hoppet;
    const double N0err = 0.003;
    const double N1err = 0.03;
    const double N2err = 0.03;
    const double N3err = 0.028;
    const double N4err = 0.04;
    const double N5err = 0.04;
    const double N6err = 0.03;
    const double N7err = 0.04;
    const double N8err = 0.04;
    const double N9err = 0.04;
    const double N10err = 0.04;
    const double N11err = 0.04;
    const double N12err = 0.25;
    const double N13err = 0.25;
    const double N14err = 0.25;
    const double N16err = 0.15;
    const double N17err = 0.10;
    int count;
};
// ---------------------------------------------------------

#endif
