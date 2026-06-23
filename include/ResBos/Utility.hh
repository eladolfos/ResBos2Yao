#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <cmath>
#include <functional>
#include <map>
#include <vector>
#include <array>
#include <algorithm>
#include <iterator>
#include <limits>

#define minLevel 10
#define maxLevel 100

const double inf = std::numeric_limits<double>::infinity();

namespace Utility {

    inline double BesJ0(const double &x){return j0(x);}
    inline double BesJ1(const double &x){return j1(x);}
    inline double BesY0(const double &x){return y0(x);}

    static const long Npts = 50;
    const double bernoulli[Npts] = {
        1, -0.5               , 0.16666666666666666     , 0,
        -0.03333333333333333  , 0, 0.023809523809523808 , 0,
        -0.03333333333333333  , 0, 0.07575757575757576  , 0,
        -0.2531135531135531   , 0, 1.1666666666666667   , 0,
        -7.092156862745098    , 0, 54.971177944862156   , 0,
        -529.1242424242424    , 0, 6192.123188405797    , 0,
        -86580.25311355312    , 0, 1.4255171666666667e6 , 0,
        -2.7298231067816094e7 , 0, 6.015808739006424e8  , 0,
        -1.5116315767092157e10, 0, 4.296146430611667e11 , 0,
        -1.3711655205088334e13, 0, 4.883323189735932e14 , 0,
        -1.9296579341940068e16, 0, 8.416930475736827e17 , 0,
        -4.0338071854059454e19, 0, 2.1150748638081993e21, 0,
        -1.2086626522296526e23, 0  
    };
    const double fac_inv[Npts] = {
        1                     , 0.5                   , 0.16666666666666666   ,
        0.041666666666666664  , 0.008333333333333333  , 0.001388888888888889  ,
        0.0001984126984126984 , 0.0000248015873015873 , 2.7557319223985893e-6 ,
        2.755731922398589e-7  , 2.505210838544172e-8  , 2.08767569878681e-9   ,
        1.6059043836821613e-10, 1.1470745597729725e-11, 7.647163731819816e-13 ,
        4.779477332387385e-14 , 2.8114572543455206e-15, 1.5619206968586225e-16,
        8.22063524662433e-18  , 4.110317623312165e-19 , 1.9572941063391263e-20,
        8.896791392450574e-22 , 3.8681701706306835e-23, 1.6117375710961184e-24,
        6.446950284384474e-26 , 2.4795962632247972e-27, 9.183689863795546e-29 ,
        3.279889237069838e-30 , 1.1309962886447718e-31, 3.7699876288159054e-33,
        1.2161250415535181e-34, 3.800390754854744e-36 , 1.151633562077195e-37 ,
        3.387157535521162e-39 , 9.67759295863189e-41  , 2.688220266286636e-42 ,
        7.265460179153071e-44 , 1.911963205040282e-45 , 4.902469756513544e-47 ,
        1.225617439128386e-48 , 2.9893108271424046e-50, 7.117406731291439e-52 ,
        1.6552108677421951e-53, 3.7618428812322616e-55, 8.359650847182804e-57 ,
        1.817315401561479e-58 , 3.866628513960594e-60 , 8.055476070751238e-62 ,
        1.643974708316579e-63 , 3.2879494166331584e-65
    };

    // constexpr version of pow for integer powers
    template<typename T>
    constexpr T ipow(T num, size_t pow) {
        return pow == 0 ? 1 : num * ipow(num, pow-1);
    }

    // constexpr factorial
    template<typename T>
    constexpr T factorial(T x) {
        return x == 0 ? 1 : x * factorial(x-1);
    }

    double zeta(const double &n);
    double xLi(const long &n, const double &x);
    double Polint(const std::vector<double> &x_, const std::vector<double> &y_,
                  const size_t &n, const double &x);

    inline void PrintProgBar(const size_t &percent) {
        static std::string bar;
        static double percentOld;
        for(size_t i = 0; i < 50; i++) {
            if(static_cast<double>(i) < percentOld/2) 
                continue;
            else if(static_cast<double>(i) < static_cast<double>(percent)/2) 
                bar.replace(i,1,"=");
            else if(static_cast<double>(i) == static_cast<double>(percent)/2) 
                bar.replace(i,1,">");
            else 
                bar.replace(i,1," ");
        }
        percentOld = static_cast<double>(percent);

        std::cout << "\r [" << bar << "] ";
        std::cout.width(3);
        std::cout << percent << "%    " << std::flush;
    }

    template<typename T>
    class LogspaceGen {
    private:
        T curValue, base;
    
    public:
        LogspaceGen(T first, T base_) : curValue(first), base(base_) {}
    
        T operator()() {
            T retval = curValue;
            curValue *= base;
            return retval;
        }
    };

    inline std::vector<double> Logspace(double start, double stop, size_t num = 50, double base = 10) {
        double realStart = pow(base, start);
        double realBase = pow(base, (stop-start)/(static_cast<double>(num)-1));

        std::vector<double> retval;
        retval.reserve(num);
        std::generate_n(std::back_inserter(retval), num, Utility::LogspaceGen<double>(realStart,realBase));
        return retval;
    } 

    template<typename T>
    class LinspaceGen {
    private:
        T curValue, base;
    
    public:
        LinspaceGen(T first, T base_) : curValue(first), base(base_) {}
    
        T operator()() {
            T retval = curValue;
            curValue += base;
            return retval;
        }
    };

    inline std::vector<double> Linspace(double start, double stop, size_t num = 50) {
        double step = (stop-start)/(static_cast<double>(num)-1);

        std::vector<double> retval;
        retval.reserve(num);
        std::generate_n(std::back_inserter(retval), num, 
                        Utility::LinspaceGen<double>(start,step));
        return retval;
    } 


}

#endif
