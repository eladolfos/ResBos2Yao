// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#ifndef __BAT__NONPERTFIT__H
#define __BAT__NONPERTFIT__H

#include <BAT/BCModel.h>

#include <memory>
#include <string>
#include <vector>

#include <TH1D.h>

#include "NonPertFit/Experiment.hh"

namespace Fitting {

class Parameter {
    public:
        Parameter() = default;

        bool IsFixed() const { return fixed; }
        std::string Prior() const { return prior; }
        std::vector<double> PriorParams() const { return prior_params; }
        double Value() const { return fixed_val; }
        std::string Name() const { return name; }
        double Min() const { return min_val; }
        double Max() const { return max_val; }
        std::string LatexName() const { return latexName; }
        bool IsNusiance() const { return nusiance; }

        friend struct YAML::convert<Parameter>;

    private:
        std::string name{};
        double fixed_val{};
        double min_val{}, max_val{};
        bool fixed{}, nusiance{};
        std::string prior{};
        std::vector<double> prior_params{};
        std::string latexName{};
};

}

// Include stuff needed for calculation
namespace ResBos {
    class ResBos;
}

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
    void CalculateObservables(const std::vector<double>&) {};
    void CalculateCorrelationMatrix() const;
    
    // ResBos calculation tools
    void InitResBos();
    void ParseParameters();
    // void InitNuisanceParameters(std::vector<);

    // Tools for calculating LogLikelihood
    double Simpson(const size_t& nx, const double& dx, const std::vector<double>& f, double& err);
    double GetValueDSigmaE288(const size_t&, const double&, const double&, const double&);
    double GetValueDSigmaDPT(const size_t&, const double&, const double&, const double&);
    std::map<std::string, double> Parameters(const std::vector<double> &pars) const;

    // Vector to hold experiments
    std::map<size_t, std::shared_ptr<Experiment>> experiments;
    static std::unique_ptr<IO::Settings> global_settings;

private:
    std::vector<Fitting::Parameter> params;
    size_t count;
    std::vector<double> inv_cov;
};
// ---------------------------------------------------------

#endif
