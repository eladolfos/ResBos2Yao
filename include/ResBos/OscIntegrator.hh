#ifndef OSCINTEGRATOR_HH
#define OSCINTEGRATOR_HH

#include <memory>
#include <vector>

namespace Utility {

class BaseIntegrator;

class OscIntegrator {
    public:
        OscIntegrator(std::unique_ptr<BaseIntegrator>, const std::vector<double>&);

        double Integrate(const double&, double, double);
        std::vector<double> IntegrateVec(const double&, double, double);

    private:
        std::unique_ptr<BaseIntegrator> m_integrator;
        std::vector<double> m_zeros;
};

}

#endif
