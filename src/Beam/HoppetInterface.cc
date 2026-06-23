#include <functional>
#include "hoppet_v1.h"
#include "ResBos/HoppetInterface.hh"

namespace Utility {

    // Need a global instance of the pdf in order to initialize Hoppet correctly. Put in the namespace to avoid
    // issues in other parts of the code
    LHAPDF::PDF* Instance;
    std::mutex Hoppet::hoppetMutex;

    // Clean up Hoppet
    Hoppet::~Hoppet() {}

    // Function needed to pass the output of LHAPDF6 to Hoppet
    void LHAsub(const double& x, const double& Q, double *res) {
        std::vector<double> xfxVec;
        Instance -> xfxQ(x,Q,xfxVec);
        for(std::size_t i = 0; i < xfxVec.size(); ++i)
            res[i] = xfxVec[i];
    }

    // Initialize Hoppet
    //      Input: PDF, charm mass, bottom mass, top mass
    Hoppet::Hoppet(LHAPDF::PDF *p) {
        Instance = p;

        const double ymax = 20.0;
        constexpr double dy = 0.1;
        const double Qmin = Instance -> qMin();
        const double Qmax = Instance -> qMax();
        constexpr double dlnlnQ = dy/4;
        constexpr int nloop = 3;
        constexpr int order = -6;
        constexpr int factscheme = 1;
        hoppetStartExtended(ymax,dy,Qmin,Qmax,dlnlnQ,nloop,order,factscheme);

        const double mc = Instance -> info().get_entry_as<double>("MCharm");
        const double mb = Instance -> info().get_entry_as<double>("MBottom");
        constexpr double mt = 1E99; // Instance -> info().get_entry_as<double>("MTop");
        hoppetSetPoleMassVFN(mc,mb,mt);

        const double asQ0 = Instance -> info().get_entry_as<double>("AlphaS_MZ");
        const double Q0alphas = Instance -> info().get_entry_as<double>("MZ"); 
        const int asOrder = Instance -> info().get_entry_as<int>("AlphaS_OrderQCD");
        hoppetPreEvolve(asQ0, Q0alphas, asOrder+1, 1, Qmin); 
        hoppetCachedEvolve(LHAsub);
    }

    // Function to switch the PDF set and restart the internal Hoppet grid to allow for PDF error sets
    void Hoppet::SwitchSet(LHAPDF::PDF *p) {
        Instance = p;
        const double asQ0 = Instance -> info().get_entry_as<double>("AlphaS_MZ");
        const double Q0alphas = Instance -> info().get_entry_as<double>("MZ"); 
        const int asOrder = Instance -> info().get_entry_as<int>("AlphaS_OrderQCD");
        hoppetPreEvolve(asQ0, Q0alphas, asOrder+1, 1, Instance -> qMin()); 
        hoppetCachedEvolve(LHAsub);
    }

    // Function that interfaces between using 3 one digit numbers to a 3 digit number needed by Hoppet to obtain the different convolutions
    // with the splitting functions
    //      Input: energy, partonic x, first splitting function, (optional) second splitting function, (optional) third splitting function
    //      Output: a map of the splitting function results for each parton
    std::map<int, double> Hoppet::GetConvolution(const double &x, const double &Q, const int &i,
                                                 const int &j, const int &k) {
        int nloop = 100*k+10*j+i;
        double f[13];
        std::map<int,double> result;

        hoppetEvalSplit(x,Q,nloop,-1,f);
        for(int l = 0; l < 13; l++) result[l-6] = f[l]/x;
        return result;
    }

    // Function that interfaces between using 3 one digit numbers to a 3 digit number needed by Hoppet to obtain the different convolutions
    // with the splitting functions
    //      Input: energy, partonic x, first splitting function, (optional) second splitting function, (optional) third splitting function
    //      Output: splitting function result for a given parton
    double Hoppet::GetConvolution(const int &pid, const double &x, const double &Q,
                                  const int &i, const int &j, const int &k) {
        double f[13];
        int nloop = 100*k+10*j+i;
        hoppetEvalSplit(x,Q,nloop,-1,f);
        return f[pid+6]/x;
    }

    double Hoppet::GetConvolution(const int &pid, const double &x, const double &Q,
                                  const Splitting &split) {
        double f[13];
        int nloop = static_cast<int>(split);
        hoppetEvalSplit(x,Q,nloop,-1,f);
        return f[pid+6]/x;
    }

}
