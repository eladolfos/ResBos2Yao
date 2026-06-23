#include <cmath>

#include "ResBos/BaseIntegrator.hh"
#include "ResBos/OscIntegrator.hh"

using namespace Utility;

OscIntegrator::OscIntegrator(std::unique_ptr<BaseIntegrator> integrator,
                             const std::vector<double> &zeros) 
    : m_integrator(std::move(integrator)), m_zeros(zeros) {}

double OscIntegrator::Integrate(const double &eps, double aerr, double rerr) {
    int steps = static_cast<int>(std::ceil(-1.31*log10(eps)));
    double d = pow(3.0+sqrt(8), steps);
    d = (d+1/d)/2;
    double b = -1, c = -d, s = 0;
    for(int i = 0; i < steps; ++i) {
        size_t idx = static_cast<size_t>(i);
        c = b - c;
        if(i % 2 == 0) s += c*m_integrator -> Integrate(m_zeros[idx], m_zeros[idx+1], aerr, rerr);
        else s -= c*m_integrator -> Integrate(m_zeros[idx], m_zeros[idx+1], aerr, rerr);
        b = (i+steps)*(i-steps)*b/((i+0.5)*(i+1));
    }

    return s/d;
}

std::vector<double> OscIntegrator::IntegrateVec(const double &eps, double aerr, double rerr) {
    int steps = static_cast<int>(std::ceil(-1.31*log10(eps)));
    double d = pow(3.0+sqrt(8), steps);
    d = (d+1/d)/2;
    double b = -1, c = -d;
    std::vector<double> s;
    for(int i = 0; i < steps; ++i) {
        size_t idx = static_cast<size_t>(i);
        c = b - c;
        auto result = m_integrator -> IntegrateVec(m_zeros[idx], m_zeros[idx+1], aerr, rerr);
        if(i == 0) s.resize(result.size());
        for(size_t j = 0; j < result.size(); ++j) {
            if(i % 2 == 0) 
                s[j] += c*result[j]/d;
            else 
                s[j] -= c*result[j]/d;
            b = (i+steps)*(i-steps)*b/((i+0.5)*(i+1));
        }
    }

    return s;
}
