// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include <BAT/BCGaussianPrior.h>
#include <BAT/BCMath.h>
#include "NonPertFit/NonPertFit.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/Grid2D.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"
#include "ResBos/PhaseSpace.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Utility.hh"
#include "fmt/format.h"

#include "TH2.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "yaml-cpp/yaml.h"
#pragma GCC diagnostic pop

#include <iostream>

double pyalem(double q2){
// Returns the running electromagnetic coupling alpha
//PN Stolen shamelessly from ResBos
//
//...Calculate real part of photon vacuum polarization.
//...For leptons simplify by using asymptotic (q^2 >> m^2) expressions.
//   For hadrons use parametrization of H. Burkhardt et al.
//   See R. Kleiss et al, CERN 89-08, vol. 3, pp. 129-131.
//csb___see also Phys.Lett.B356:398-403,1995

  double pi, alpha0, rpigg, aempi;
  pi = 3.141592654;
  alpha0=1./137.04;
  aempi= alpha0/(3.*pi);

  if (q2 < 2e-6)
    rpigg =0.0;
  else if(q2 < 0.09)
    rpigg = aempi*(13.4916 + log(q2)) + 0.00835*log(1.+q2);
  else if(q2 < 9.)
    rpigg=aempi*(16.3200+2.0*log(q2))+ 0.00238*log(1.0+3.927*q2);
  else if(q2 < 1e4)
    rpigg=aempi*(13.4955+3.0*log(q2))+0.00165+ 0.00299*log(1.0+q2);
  else
    rpigg=aempi*(13.4955+3.0*log(q2))+ 0.00221 + 0.00293*log(1.0+q2);

//...Calculate running alpha_em.
  return alpha0/(1.0-rpigg);
}//pyalem ->

std::unique_ptr<IO::Settings> NonPertFit::global_settings = nullptr;

namespace YAML {
template<>
struct convert<Fitting::Parameter> {
    static Node encode(const Fitting::Parameter &param) {
        Node node;
        node.push_back(param.name);
        node.push_back(param.fixed_val);
        node.push_back(param.min_val);
        node.push_back(param.max_val);
        node.push_back(param.prior);
        node.push_back(param.prior_params);
        node.push_back(param.latexName);
        node.push_back(param.fixed);

        return node;
    }

    static bool decode(const Node &node, Fitting::Parameter &param) {
        if(!node.IsSequence() || node.size() != 8) return false;
        param.name = node[0].as<std::string>();
        param.fixed_val = node[1].as<double>();
        param.min_val = node[2].as<double>();
        param.max_val = node[3].as<double>();
        param.prior = node[4].as<std::string>();
        param.prior_params = node[5].as<std::vector<double>>();
        param.latexName = node[6].as<std::string>();
        param.fixed = !node[7].as<bool>();

        return true;
    }
};

template<>
struct convert<ExpType> {
    static bool decode(const Node &node, ExpType &type) {
        auto name = node.as<std::string>();
        if(name == "collider") type = ExpType::Collider;
        else if(name == "fixed") type = ExpType::Fixed;
        else if(name == "fixed2") type = ExpType::Fixed2;
        else throw std::runtime_error("Invalid Experiment type" + name + "\n");

        return true;
    }
};

template<>
struct convert<Experiment> {
    static bool decode(const Node &node, Experiment &experiment) {
        if(!node.IsSequence() || node.size() != 8) return false;
        auto id = node[0].as<size_t>();
        auto name = node[1].as<std::string>();
        experiment.experiment = node[2].as<ExpType>();

        auto filename = node[3].as<std::string>();
        DataSet _data(filename); 
        if(id != _data.ID()) {
            std::cout << "ID values do not match: " << id << " and " << _data.ID() << "\n";
            return false;
        }
        if(name != _data.Name()) {
            std::cout << "Names do not match: " << name << " and " << _data.Name() << "\n";
            return false;
        }
        experiment.data = _data;

        experiment.ptYInc = node[4].as<double>();
        experiment.yk = node[5].as<bool>();
        experiment.fit = node[6].as<bool>();
        experiment.calculate = node[7].as<bool>();

        return true;
    }
};

}

void NonPertFit::ParseParameters() {
    YAML::Node paramYAML = YAML::LoadFile(global_settings -> GetSettingString("ParameterFile"));
    auto parameters = paramYAML["Parameters"];
    for(auto parameter : parameters) {
        params.emplace_back(parameter["parameter"].as<Fitting::Parameter>());
        auto param = params.back();
        if(!param.IsFixed()) {
            AddParameter(param.Name(), param.Min(), param.Max(), param.LatexName(), "");
            if(param.Prior() == "constant") GetParameters().Back().SetPriorConstant();
            else if(param.Prior() == "gaussian") {
                auto prior_params = param.PriorParams();
                GetParameters().Back().SetPrior(
                        new BCGaussianPrior(prior_params[0], prior_params[1]));
            } else throw std::runtime_error("Invalid prior " + param.Prior() + '\n');
        }
    }
}

// ---------------------------------------------------------
NonPertFit::NonPertFit(const std::string& name)
    : BCModel(name)
{

    ParseParameters();
    InitResBos();

    // Define observables here, too. For example:
    //AddObservable("qt_dist", 0, 100, "#mu^{2}", "[GeV^{2}]");

    count = 0;
}

// ---------------------------------------------------------
NonPertFit::~NonPertFit()
{
}

std::map<std::string, double> NonPertFit::Parameters(const std::vector<double> &pars) const {
    size_t idx = 0;
    std::map<std::string, double> result;
    for(const auto &param : params) {
        if(param.IsFixed()) result[param.Name()] = param.Value();
        else result[param.Name()] = pars[idx++];
    }

    return result;
}

// ---------------------------------------------------------
double NonPertFit::LogLikelihood(const std::vector<double>& pars)
{
    std::map<std::string, double> m_pars = Parameters(pars);

    // return the log of the conditional probability p(data|pars).
    // This is where you define your model.
    // BCMath contains many functions you will find helpful.
    size_t nParams = global_settings -> GetSetting<std::size_t>("NParams");
    std::vector<double> g;
    for(size_t iParam = 0; iParam < nParams; ++iParam) {
        g.push_back(m_pars[fmt::format("g{}", iParam+1)]);
    }

    double bMax = 0.0;//yfu
    if(m_pars.count("bMax")) bMax = m_pars["bMax"];

    double yMax = 5.0;//yfu
    if(m_pars.count("yMax")) yMax = m_pars["yMax"];

    double logl = 0;
    double prediction = 0;
    size_t lastType = 0;
    bool reset = false;
    for(auto experiment : experiments) {
        if(!experiment.second -> ToFit()) continue;
        ++count;
        experiment.second -> SetGees(g);
        //if(m_pars.count("bMax")) experiment.second ->SetBMax(bMax);//yfu
        //if(m_pars.count("yMax")) experiment.second -> SetYMax(yMax);//yfu
        if((count % global_settings -> GetSetting<size_t>("AvgUpdateRate") == 0)
            && count < 5000) {
            reset = true;
        }
        double chi2{};
        try {
            chi2 = experiment.second -> Chi2(reset);
        } catch(std::runtime_error &e) {
            return -1e99;
        }
        logl += chi2;
    }

    return -0.5*logl;
}

// ---------------------------------------------------------
void NonPertFit::InitResBos() {
    YAML::Node expYAML = YAML::LoadFile(global_settings -> GetSettingString("ExperimentFile"));
    for(auto experiment : expYAML["Experiments"]) {
        std::cout << "Initializing: " << experiment["experiment"][1].as<std::string>() << "\n";
        auto tmp = std::shared_ptr<Experiment>(new Experiment(experiment["experiment"].as<Experiment>()));
        experiments[tmp -> Id()] = tmp;
        experiments[tmp -> Id()] -> Init();
    }
}

void NonPertFit::CalculateCorrelationMatrix() const {
    BCLog::Out(BCLog::LogLevel::summary, fmt::format(""));
    BCLog::Out(BCLog::LogLevel::summary,
                fmt::format(" Correlation Matrix"));
    BCLog::Out(BCLog::LogLevel::summary,
                fmt::format(" =================="));
    BCLog::Out(BCLog::LogLevel::summary,
                fmt::format(" List of correlations between the variables:"));
    BCLog::Out(BCLog::LogLevel::summary, fmt::format(""));
    for(unsigned i = 0; i < GetNVariables(); ++i) {
        double var_i = (i < fMCMCStatistics_AllChains.variance.size())
            ? fMCMCStatistics_AllChains.variance[i]
            : std::numeric_limits<double>::infinity();
        for(unsigned j = i+1; j < GetNVariables(); ++j) {
            double var_j = (j < fMCMCStatistics_AllChains.variance.size())
                ? fMCMCStatistics_AllChains.variance[j]
                : std::numeric_limits<double>::infinity();
            double covar_ij = (i < fMCMCStatistics_AllChains.covariance.size()
                    && j < fMCMCStatistics_AllChains.covariance[i].size())
                ? fMCMCStatistics_AllChains.covariance[i][j]
                : std::numeric_limits<double>::infinity();
            double corr_ij = std::numeric_limits<double>::infinity();
            if(std::isfinite(covar_ij) && std::isfinite(var_i) && std::isfinite(var_j))
                corr_ij = covar_ij / sqrt(var_i * var_j);
            else if (i < fH2Marginalized.size() && j < fH2Marginalized[i].size()
                    && fH2Marginalized[i][j])
                corr_ij = fH2Marginalized[i][j] -> GetCorrelationFactor();
            else if (j < fH2Marginalized.size() && i < fH2Marginalized[j].size()
                    && fH2Marginalized[j][i])
                corr_ij = fH2Marginalized[j][i] -> GetCorrelationFactor();
            BCLog::Out(BCLog::LogLevel::summary,
                        fmt::format("  Corr({}, {}) = {}",
                        GetVariable(i).GetLatexNameWithUnits().data(),
                        GetVariable(j).GetLatexNameWithUnits().data(),
                        corr_ij));
        }
    }
    BCLog::Out(BCLog::LogLevel::summary, fmt::format(""));
}

// ---------------------------------------------------------
// double NonPertFit::LogAPrioriProbability(const std::vector<double>& pars)
// {
//     // return the log of the prior probability p(pars)
//     // If you use built-in priors, leave this function commented out.
// }

// ---------------------------------------------------------
// void NonPertFit::CalculateObservables(const std::vector<double>& pars)
// {
//     // Calculate and store obvserables. For example:
//     GetObservable(0) = pow(pars[0], 2);
// }
