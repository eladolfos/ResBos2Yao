#ifndef ASYMPTOTIC_HH
#define ASYMPTOTIC_HH

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

namespace ResBos {
    typedef std::map<Beam::BDF,double> ConvMap;

    class Asymptotic : virtual public Calculation {
        public:
            // Initialize Asymptotic class
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<Asymptotic>(new Asymptotic());
            }
            void Initialize(IO::Settings*, std::shared_ptr<ResBos>);

            static std::string GetName() { return "Asymptotic"; }
            inline std::vector<Conv> GetNeededConvs() const {
                std::vector<Conv> neededGrids;
                if(GetAsymOrder()>1) {
                    neededGrids.push_back(Conv::C1);
                    neededGrids.push_back(Conv::C1P1);
                    if(GetAsymOrder()>2) {
                        neededGrids.push_back(Conv::C2);
                        neededGrids.push_back(Conv::C1P1P1);
                        neededGrids.push_back(Conv::C1P2);
                        neededGrids.push_back(Conv::C2P1);
                    }
                }
                return neededGrids;
            }
            bool IsRegistered() { return registered; }

            // Calculate Asymptotic piece
            virtual std::vector<double> GetCalc(const double&, const double&, const double&);

        protected:
            Asymptotic() {};
            static bool registered;

        private:
            virtual std::vector<double> CalcTerms(const double&, const double&,
                                                  const double&, const double&) const; 

            // alpha_s Asymptotic functions
            double C11(const size_t&, const ConvMap&, const ConvMap&) const;
            double C10(const size_t&, const ConvMap&, const ConvMap&) const;
            
            // alpha_s^2 Asymptotic functions
            double C23(const size_t&, const ConvMap&, const ConvMap&) const;
            double C22(const size_t&, const ConvMap&, const ConvMap&) const;
            double C21(const size_t&, const ConvMap&, const ConvMap&) const;
            double C20(const size_t&, const ConvMap&, const ConvMap&) const;

            // alpha_s^3 Asymptotic functions
            double C35(const size_t&, const ConvMap&, const ConvMap&) const;
            double C34(const size_t&, const ConvMap&, const ConvMap&) const;
            double C33(const size_t&, const ConvMap&, const ConvMap&) const;
            double C32(const size_t&, const ConvMap&, const ConvMap&) const;
            double C31(const size_t&, const ConvMap&, const ConvMap&) const;
            double C30(const size_t&, const double&, const ConvMap&, const ConvMap&) const;

            // Scale Dependent functions
            //   alpha_s^2 terms
            double C21log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
            double C20log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;

            //   alpha_s^3 terms
            double C33log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
            double C32log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
            double C31log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
            double C30log(const size_t&, const ConvMap&, const ConvMap&,
                          const double&, const double&) const;
    };
}

#endif
