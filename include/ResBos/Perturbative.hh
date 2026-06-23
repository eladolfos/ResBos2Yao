#ifndef PERTURBATIVE_HH
#define PERTURBATIVE_HH

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

    class Perturbative : virtual public Calculation {
        public:
            // Initialize Perturbative class
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<Perturbative>(new Perturbative());
            }
            void Initialize(IO::Settings*, std::shared_ptr<ResBos>);

            static std::string GetName() {return "Perturbative";}
            std::vector<Conv> GetNeededConvs() const {return std::vector<Conv>(0);}
            bool IsRegistered() {return registered;}

            // Calculate Perturbative piece
            virtual std::vector<double> GetCalc(const double&, const double&, const double&);

        protected:
            Perturbative() {};
            static bool registered;

        private:
            double X1Integral(const double&, const double&, const double&, const double&,
                              const double&, const double&, const size_t&) const;
            double X2Integral(const double&, const double&, const double&, const double&,
                              const double&, const double&, const size_t&) const;

    };
}

#endif
