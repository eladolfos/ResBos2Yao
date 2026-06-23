#ifndef QCDCONST_HH
#define QCDCONST_HH

#include <cmath>

namespace QCD {
    constexpr double CA = 3.0;
    constexpr double CF = (CA*CA-1)/(2*CA);
    constexpr double beta0(const size_t &nf) { return (11*CA-2*static_cast<double>(nf))/12.0; }
    constexpr double beta1(const size_t &nf) { 
        return (17*CA*CA-(5*CA+3*CF)*static_cast<double>(nf))/24.0; 
    }
    constexpr double beta2(const size_t &nf) {
        return (2857.0/3456.0*CA*CA*CA-1415.0/3456.0*CA*CA*static_cast<double>(nf)
                +79.0/3456.0*CA*static_cast<double>(nf)*static_cast<double>(nf)
                +11.0/576.0*CF*static_cast<double>(nf)*static_cast<double>(nf)
                -205.0/1152.0*CF*CA*static_cast<double>(nf)+CF*CF*static_cast<double>(nf)/64.0);
    }
    constexpr double ZETA3 = 1.2020569031595942854;
    constexpr double ZETA5 = 1.0369277551433699263;
    constexpr double pi2 = M_PI*M_PI;
    constexpr double hbarc2 = 0.38937966E9;
    constexpr double EulerGamma = 0.5772156649015328606;
    const double B0 = 2*exp(-EulerGamma);
}

#endif
