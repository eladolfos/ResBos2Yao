#ifndef ADZINT_HH
#define ADZINT_HH

#include <array>

#include "ResBos/BaseIntegrator.hh"

namespace Utility {

class Adzint : public BaseIntegrator {
    public:
        Adzint(const int&, const int&, bool cache=true);
        Adzint(const int&, const int&, const FunctionD&, bool cache=true);
        Adzint(const int&, const int&, const FunctionVD&, bool cache=true);

        double Integrate(const double&, const double&, double&, double&);
        std::vector<double> IntegrateVec(const double&, const double&,
                                         double&, double&);

    private:
        // Private functions
        std::pair<double, double> Sglint(const int&, const double&, const double&,
                                         const double&, const double&);
        void Privcal(const size_t&, double&, double&);
        void Privspl(const size_t&);

        // Variables
        int iacta, iactb;
        static constexpr size_t maxint = 5000;
        std::array<double, maxint> err, u, v, result, fu, fv, fw;
        double fa, fb, res, ers;
        size_t ib, ier, scaleFac, numint;
        static const double sml, huge, tiny;
};

}

#endif
