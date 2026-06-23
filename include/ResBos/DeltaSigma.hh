#ifndef DELTASIGMA_HH
#define DELTASIGMA_HH

#include <vector>
#include <map>

#include "ResBos/Calculation.hh"
#include "ResBos/Enums.hh"

namespace IO {
    class Settings;
}

namespace Beam {
    enum class BDF;
}

typedef std::map<Beam::BDF,double> ConvMap;
namespace ResBos {
    class ResBos;

    class DeltaSigma : public Calculation {
        public:
            // Initialize Asymptotic class
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<DeltaSigma>(new DeltaSigma());
            }
            void Initialize(IO::Settings*,std::shared_ptr<ResBos>);

            static std::string GetName() { return "DeltaSigma"; }
            std::vector<Conv> GetNeededConvs() const;
            bool IsRegistered() { return registered; }

            // Calculate Asymptotic piece
            virtual std::vector<double> GetCalc(const double&, const double&, const double&);

        private:
            DeltaSigma() {};
            static bool registered;

            virtual std::vector<double> CalcTerms(const double&, const double&,
                                                  const double&, const double&) const; 

            // alpha_s DeltaSigma functions
            double V12(const size_t&, const ConvMap&, const ConvMap&) const;
            double V11(const size_t&, const ConvMap&, const ConvMap&) const;
            double V10(const size_t&, const ConvMap&, const ConvMap&) const;
            
            // alpha_s^2 DeltaSigma functions
            double V24(const size_t&, const ConvMap&, const ConvMap&) const;
            double V23(const size_t&, const ConvMap&, const ConvMap&) const;
            double V22(const size_t&, const ConvMap&, const ConvMap&) const;
            double V21(const size_t&, const ConvMap&, const ConvMap&) const;
            double V20(const size_t&, const ConvMap&, const ConvMap&) const;

            // alpha_s^3 DeltaSigma functions
            double V36(const size_t&, const ConvMap&, const ConvMap&) const;
            double V35(const size_t&, const ConvMap&, const ConvMap&) const;
            double V34(const size_t&, const ConvMap&, const ConvMap&) const;
            double V33(const size_t&, const ConvMap&, const ConvMap&) const;
            double V32(const size_t&, const ConvMap&, const ConvMap&) const;
            double V31(const size_t&, const double&, const ConvMap&, const ConvMap&) const;
            double V30(const size_t&, const double&, const ConvMap&, const ConvMap&) const;

            // Scale Dependent functions
            //   alpha_s terms
            double V10log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;

            //   alpha_s^2 terms
            double V22log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
            double V21log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
            double V20log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
    };
}

#endif
