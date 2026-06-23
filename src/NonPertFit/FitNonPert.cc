// ***************************************************************
// This file was created using the bat-project script
// for project NonPert.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include <BAT/BCLog.h>
#include <BAT/BCAux.h>

#include "NonPertFit/NonPertFit.hh"
#include "ResBos/loguru.hpp"
#include "NonPertFit/Dataset.hh"

int main(int argc, char *argv[])
{
    std::string fitting_config = "fitting.config";
    if(argc == 2) fitting_config = argv[1];
    // Startup logger
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;

    // open log file
    BCLog::OpenLog("log.txt", BCLog::detail, BCLog::detail);
    BCLog::OutSummary(fmt::format("Using {} as config file", fitting_config));
    NonPertFit::global_settings = std::unique_ptr<IO::Settings>(new IO::Settings(fitting_config));

    // create new NonPertFit object
    NonPertFit m(NonPertFit::global_settings -> GetSettingString("FitName"));

    // Set random seed
    m.SetRandomSeed(NonPertFit::global_settings -> GetSetting<unsigned int>("Seed"));

    // set precision
    std::string precision = NonPertFit::global_settings -> GetSettingString("Precision");
    if(precision == "Quick") m.SetPrecision(BCEngineMCMC::kQuick);
    else if(precision == "Medium") m.SetPrecision(BCEngineMCMC::kMedium);
    else if(precision == "High") m.SetPrecision(BCEngineMCMC::kHigh);
    else if(precision == "VeryHigh") m.SetPrecision(BCEngineMCMC::kVeryHigh);

    BCLog::OutSummary("Test model created");

    // Normalize the posterior by integrating it over the full parameter space
    // m.Normalize();

    // Write Markov Chain to a ROOT file as a TTree
    // m.WriteMarkovChain(m.GetSafeName() + "_mcmc.root", "RECREATE");

    // run MCMC, marginalizing posterior
    m.MarginalizeAll(BCIntegrate::kMargMetropolis);

    // run mode finding; by default using Minuit
    m.SetOptimizationMethod(BCIntegrate::kOptMinuit);
    m.FindMode(m.GetBestFitParameters());

    // draw all marginalized distributions into a PDF file
    m.PrintAllMarginalized(m.GetSafeName() + "_plots.pdf");

    // print summary plots
    m.PrintParameterPlot(m.GetSafeName() + "_parameters.pdf");
    m.PrintCorrelationPlot(m.GetSafeName() + "_correlation.pdf");
    m.PrintCorrelationMatrix(m.GetSafeName() + "_correlationMatrix.pdf");
    m.PrintKnowledgeUpdatePlots(m.GetSafeName() + "_update.pdf");

    // print results of the analysis into a text file
    m.PrintSummary();
    m.CalculateCorrelationMatrix();

    // Check data one last time
    std::vector<double> pars = m.GetBestFitParameters();
    std::map<std::string, double> m_pars = m.Parameters(pars);
    for(auto m_par : m_pars) {
        BCLog::Out(BCLog::LogLevel::summary, fmt::format("{} = {}", m_par.first, m_par.second));
    }
    std::vector<double> g{m_pars["g1"], m_pars["g2"], m_pars["g3"], m_pars["g4"], m_pars["g5"], m_pars["g6"]};
    std::vector<double> chi2;
    for(auto experiment : m.experiments) {
        experiment.second -> SetGees(g);
        BCLog::Out(BCLog::LogLevel::summary, "");
        BCLog::Out(BCLog::LogLevel::summary,
                   fmt::format(" Experiment: {}", experiment.second -> Name()));
        BCLog::Out(BCLog::LogLevel::summary,
                   fmt::format(" ============{}",
                       std::string(experiment.second -> Name().size(), '=')));
        chi2.emplace_back(experiment.second -> Chi2(true, true));
        BCLog::Out(BCLog::LogLevel::summary, "");
    }

    BCLog::Out(BCLog::LogLevel::summary, " Final Result");
    BCLog::Out(BCLog::LogLevel::summary, " ============");
    size_t totalPts = 0;
    BCLog::Out(BCLog::LogLevel::summary, "   Results Per Experiment:");
    BCLog::Out(BCLog::LogLevel::summary, "   -----------------------");
    size_t idx = 0;
    double sum_chi2 = 0;
    for(auto exp : m.experiments) {
        BCLog::Out(BCLog::LogLevel::summary, fmt::format("     Experiment: {}", exp.second -> Name()));
        BCLog::Out(BCLog::LogLevel::summary, fmt::format("     ------------{}", std::string(exp.second -> Name().size(), '-'))); 
        sum_chi2 += chi2[idx];
        BCLog::Out(BCLog::LogLevel::summary, fmt::format("       Chi2: {:.3f}", chi2[idx]));
        BCLog::Out(BCLog::LogLevel::summary, fmt::format("       Pts:  {}", exp.second -> NPoints()));
        BCLog::Out(BCLog::LogLevel::summary, "");
        totalPts += exp.second -> NPoints();
        ++idx;
    }
    BCLog::Out(BCLog::LogLevel::summary, "");
    BCLog::Out(BCLog::LogLevel::summary, "   Total Results:");
    BCLog::Out(BCLog::LogLevel::summary, "   --------------");
    BCLog::Out(BCLog::LogLevel::summary, fmt::format("     Total Chi2: {:.3f}", sum_chi2));
    size_t dof = totalPts - m.GetNVariables();
    BCLog::Out(BCLog::LogLevel::summary, fmt::format("     N_dof: {}", dof));
    BCLog::Out(BCLog::LogLevel::summary, fmt::format("     Chi2/N_dof: {:.3f}", sum_chi2/static_cast<double>(dof)));
    BCLog::Out(BCLog::LogLevel::summary, "");

    // close log file
    BCLog::OutSummary("Exiting");
    BCLog::CloseLog();

    return 0;
}
