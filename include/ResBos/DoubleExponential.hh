#ifndef DOUBLEEXPONENTIAL_HH
#define DOUBLEEXPONENTIAL_HH

#include <algorithm>
#include <array>
#include <cmath>

#include "ResBos/BaseIntegrator.hh"
#include "ResBos/Utility.hh"

namespace Utility {

// by Xeo, from https://stackoverflow.com/a/13294458/420683
template<size_t... Is> struct seq{};
template<size_t N, size_t... Is>
struct gen_seq : gen_seq<N-1, N-1, Is...>{};
template<size_t... Is>
struct gen_seq<0, Is...> : seq<Is...>{};

// Generator functions
// by dyp, from https://stackoverflow.com/a/19016627/9201027
template<class Generator, size_t... Is>
constexpr auto generate_array_helper(Generator g, seq<Is...>)
-> std::array<decltype(g(size_t{}, sizeof...(Is))), sizeof...(Is)> {
    return {{g(Is, sizeof...(Is))...}};
}

template<size_t tcount, class Generator>
constexpr auto generate_array(Generator g) 
-> decltype(generate_array_helper(g, gen_seq<tcount>{})) {
    return generate_array_helper(g, gen_seq<tcount>{});
}

template<typename T>
constexpr T xn_nfac(T x, size_t n) {
    return ipow(x, n) / factorial<T>(n);
}

// constexpr for exp using a Taylor series expansion
constexpr double cexp(const double &x) {
    return 1  + x + ipow(x, 2)/factorial<double>(2) + ipow(x, 3)/factorial<double>(3)
        + ipow(x, 4)/factorial<double>(4) + ipow(x, 5)/factorial<double>(5)
        + ipow(x, 6)/factorial<double>(6) + ipow(x, 7)/factorial<double>(7)
        + ipow(x, 8)/factorial<double>(8) + ipow(x, 9)/factorial<double>(9)
        + ipow(x, 10)/factorial<double>(10) + ipow(x, 11)/factorial<double>(11)
        + ipow(x, 12)/factorial<double>(12) + ipow(x, 13)/factorial<double>(13)
        + ipow(x, 14)/factorial<double>(14) + ipow(x, 15)/factorial<double>(15)
        + ipow(x, 16)/factorial<double>(16) + ipow(x, 17)/factorial<double>(17)
        + ipow(x, 18)/factorial<double>(18) + ipow(x, 19)/factorial<double>(19)
        + ipow(x, 20)/factorial<double>(20) + ipow(x, 21)/factorial<double>(21)
        + ipow(x, 22)/factorial<double>(22) + ipow(x, 23)/factorial<double>(23)
        + ipow(x, 24)/factorial<double>(24) + ipow(x, 25)/factorial<double>(25)
        + ipow(x, 26)/factorial<double>(26) + ipow(x, 27)/factorial<double>(27)
        + ipow(x, 28)/factorial<double>(28) + ipow(x, 29)/factorial<double>(29)
        + ipow(x, 30)/factorial<double>(30) + ipow(x, 31)/factorial<double>(31)
        + ipow(x, 32)/factorial<double>(32) + ipow(x, 33)/factorial<double>(33)
        + ipow(x, 34)/factorial<double>(34) + ipow(x, 35)/factorial<double>(35)
        + ipow(x, 36)/factorial<double>(36) + ipow(x, 37)/factorial<double>(37)
        + ipow(x, 38)/factorial<double>(38) + ipow(x, 39)/factorial<double>(39)
        + ipow(x, 40)/factorial<double>(40) + ipow(x, 41)/factorial<double>(41)
        + ipow(x, 42)/factorial<double>(42) + ipow(x, 43)/factorial<double>(43)
        + ipow(x, 44)/factorial<double>(44) + ipow(x, 45)/factorial<double>(45)
        + ipow(x, 46)/factorial<double>(46) + ipow(x, 47)/factorial<double>(47)
        + ipow(x, 48)/factorial<double>(48) + ipow(x, 49)/factorial<double>(49)
        + ipow(x, 50)/factorial<double>(50) + ipow(x, 51)/factorial<double>(51)
        + ipow(x, 50)/factorial<double>(52) + ipow(x, 51)/factorial<double>(53)
        + ipow(x, 50)/factorial<double>(54) + ipow(x, 51)/factorial<double>(55);
}

// Setup abscissa and weights for Double Exponential integration
constexpr size_t _phases = 8;
constexpr size_t _size = 6*1<<_phases;

struct DEPoint {
    double abscissa;
    double weights; 
};

constexpr double t2(const size_t &k) {
    return cexp(static_cast<double>(k)/ipow(2.0,_phases));
}

constexpr double u1(const size_t &k) {
    return 0.5*M_PI/2.0*(t2(k)+1.0/t2(k));
}

constexpr double t3(const size_t &k) {
    return cexp(0.5*M_PI/2.0*(t2(k)-1.0/t2(k)));   
}

constexpr double t4(const size_t &k) {
    return 0.5*(t3(k)+1.0/t3(k));   
}

constexpr double GetAbcs(const size_t &k) {
    return 1.0/(t3(k)*t4(k));
}

constexpr double GetWeight(const size_t &k) {
    return u1(k)/(t4(k)*t4(k));
}

constexpr DEPoint GeneratePoint(size_t curr, size_t){
    return { GetAbcs(curr) , GetWeight(curr)};
}

// Table for Double Exponential integration
static constexpr auto table = generate_array<_size>(GeneratePoint);

class DoubleExponential : public BaseIntegrator {
    public:
        DoubleExponential(bool cache=false) : BaseIntegrator(cache) {}
        DoubleExponential(const FunctionD &func, bool cache=false) : BaseIntegrator(func, cache) {}
        DoubleExponential(const FunctionVD &func, bool cache=false) 
            : BaseIntegrator(func, cache) {}

        double Integrate(const double&, const double&, double&, double&);
        std::vector<double> IntegrateVec(const double&, const double&,
                                         double&, double&);

    private:
};

}

#endif
