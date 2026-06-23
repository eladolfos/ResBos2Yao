#ifndef GAUSSKRONROD_HH
#define GAUSSKRONROD_HH

#include "ResBos/BaseIntegrator.hh"

namespace Utility {

class GaussKronrod : public BaseIntegrator {
    public:
        GaussKronrod(bool cache=true) : BaseIntegrator(cache) {}
        GaussKronrod(const FunctionD &func, bool cache=true) 
            : BaseIntegrator(func, cache) {}
        GaussKronrod(const FunctionVD &func, bool cache)
            : BaseIntegrator(func, cache) {}

        double Integrate(const double&, const double&, double&, double&);
        std::vector<double> IntegrateVec(const double&, const double&,
                                         double&, double&);

    private:
        // Variables
        static constexpr size_t knots = 7;
        static const std::vector<double> KronrodWgts, GaussWgts, absc;
};

}

#endif
