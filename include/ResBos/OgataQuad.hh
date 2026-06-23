#ifndef OGATAQUAD_HH
#define OGATAQUAD_HH

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "ResBos/BesselZeros.hh"

namespace Utility {

class OgataQuad {
    public:
        using func = std::function<double(const double&)>;
        using funcVec = std::function<std::vector<double>(const double&)>;

        OgataQuad(const int& _nu=0) : nu(_nu) { init(); }

        // Single value integrators
        double OgataT(const func&, const double&, const double& rerr = 1E-16,
                      const double& aerr = 1E-16) const;
        double OgataU(const func&, const double&, const double& rerr = 1E-16,
                      const double& aerr = 1E-16) const;
        double FBTU(const func&, const double&, const std::pair<double, double>&,
                    const double& rerr = 1E-16, const double& aerr = 1E-16) const;
        double FBT(const func&, const double&, const std::pair<double, double>&,
                   const double& rerr = 1E-16, const double& aerr = 1E-16) const;

        // Vector integrators
        std::vector<double> OgataT(const funcVec&, const double&, const double& rerr= 1E-16,
                                   const double& aerr = 1E-16) const;
        std::vector<double> FBT(const funcVec&, const double&, double, const std::pair<double, double>&,
                                const double& rerr = 1E-16, const double& aerr = 1E-16);

    private:
        void init();
        double GetHu(const funcVec&, const double&, const std::pair<double, double>&) const;
        double GetHu(const func&, const double&, const std::pair<double, double>&) const;
        double GetHt(const double&) const;
        static inline bool AbsCompare(const double& a, const double& b) noexcept {
            return (std::abs(a) < std::abs(b));
        }
        static inline double GetPsi(const double& t) {
            return t*tanh(M_PI/2*sinh(t));
        }
        static inline double GetPsiP(const double& t) {
            return M_PI*t*(-pow(tanh(M_PI*sinh(t)/2),2) + 1)*cosh(t)/2
                   + tanh(M_PI*sinh(t)/2);
        }
        int nu;
        static constexpr size_t maxN = 50000;
        static constexpr double hDefault = 0.001;
        static constexpr std::array<double, maxN> jnZeros = besselZero;
        std::array<double, maxN> xi, w;
        std::map<std::pair<double, double>, double> htMap;
};

}

#endif
