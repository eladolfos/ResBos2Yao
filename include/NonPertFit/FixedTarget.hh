#ifndef NonPertFit_FixedTarget_hh
#define NonPertFit_FixedTarget_hh

#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Process.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Utility.hh"
#include "ResBos/Vegas.hh"
#include "NonPertFit/Dataset.hh"

#include <algorithm>

namespace ResBos {
    class ResBos;
    class Process;
}

class FixedTarget
{
 public:

        std::unique_ptr<IO::Settings> global_settings;
        std::shared_ptr<ResBos::ResBos> resbos;
        std::unique_ptr<ResBos::Calculation> asym, pert;
        std::unique_ptr<ResBos::Calculation> lo;
        std::string m_beam = "pCu";
        double m_ECM = 38.8;
        std::string CalcType;

        std::vector<double> qt_interp;
        std::vector<double> pert1_interp;
        std::vector<double> pert2_interp;
        std::vector<double> yk_interp;

        FixedTarget();
        virtual void ReadFile(std::string FileName);
        virtual void Init(std::string m_beam, double m_ECM);
        virtual double GetPrediction(double y, double Q, bool is_yk = false);
        virtual double Simpson(const size_t& nx, const double& dx, const std::vector<double>& f, double& err);
        virtual double Polint(std::vector<double> InpVector, std::vector<double> FuncVector, double Xinp);
        virtual int Locate(std::vector<double> Points, double Input);
};
#endif
