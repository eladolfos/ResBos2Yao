#include <cmath>
#include <functional>

template<unsigned... Is> struct seq{};
template<unsigned N, unsigned... Is>
struct gen_seq : gen_seq<N-1, N-1, Is...>{};
template<unsigned... Is>
struct gen_seq<0, Is...> : seq<Is...>{};

const int _phases = 8;
constexpr int _size = 6*pow(2,_phases);

struct DETable {
    static constexpr int length = _size;

    double abscissa[length];
    double weights[length]; 
};

constexpr double t2(const int k) {
    return exp(k*std::ldexp(1,-_phases));
}

constexpr double u1(const int k) {
    return 0.5*M_PI/2.0*(t2(k)+1.0/t2(k));
}

constexpr double t3(const int k) {
    return exp(0.5*M_PI/2.0*(t2(k)-1.0/t2(k)));   
}

constexpr double t4(const int k) {
    return 0.5*(t3(k)+1.0/t3(k));   
}

constexpr double get_abcs(const int k) {
    return 1.0/(t3(k)*t4(k));
}

constexpr double get_weight(const int k) {
    return u1(k)/(t4(k)*t4(k));
}

template<unsigned... Is>
constexpr DETable TableGen(seq<Is...>){
  return {{ get_abcs(Is)... }, {get_weight(Is)... }};
}

constexpr DETable TableGen(){
  return TableGen(gen_seq<_size>{});
}

#include <iostream>

constexpr DETable table = TableGen();

double integrate(std::function<double(double)> func, const double x1, const double x2) {
    static const int izx = 5;
    double ax = 0.5*(x2-x1);
    double bx = 0.5*(x2+x1);
    double sum = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    double fm1 = 0.0, fm2 = 0.0, err = 0.0, h = 1.0;
    double xx1, xx2, eps1, eps2, t1, t2, tw1, tw2;

    int phases = _phases+1, k1, k2, iz1, iz2;
    int *ip = new int[phases];
    ip[0] = 1;
    for(int i = 1; i < phases; ++i) ip[i] = 2*ip[i-1];

    int evals = 0;
    for(long int k = 1; k <= _phases; ++k) {
        h *= 0.5;
        s3 = s2;
        s2 = s1;
        fm1 = 0.0;
        fm2 = 0.0;
        k1 = ip[_phases-k];
        k2 = ip[_phases-k+1];

        iz1 = iz2 = 0;

        //Evaluate function at level k in x, avoiding unnecessary computation.
        for(long int i = 0; i < _size; i += k1) {
            if(i % k2 != 0 || k == 1) {
                double xt1 = 1.0 - table.abscissa[i];
                double xx1 = bx - ax*xt1;
                double xx2 = bx + ax*xt1;
                bool log1 = xx1 > x1 && iz1 < izx;
                bool log2 = xx2 < x2 && iz2 < izx;

                if(!log1 && !log2) break;

                if(log1) {
                    t1 = func(xx1);
                    evals++;
                    tw1 = t1*table.weights[i];
                    if(std::abs(tw1) < 1E-8) iz1++;
                    else iz1 = 0;
                } else {
                    t1 = 0.0;
                    tw1 = 0.0;
                }

                if(i > 0 && log2) {
                    t2 = func(xx2);
                    evals++;
                    tw2 = t2*table.weights[i];
                    if(std::abs(tw2) < 1E-8) iz2++;
                    else iz2 = 0;
                } else {
                    t2 = 0.0;
                    tw2 = 0.0;
                }

                sum += tw1 + tw2;
                tw1 = std::abs(tw1);
                tw2 = std::abs(tw2);
                t1 = std::abs(t1);
                t2 = std::abs(t2);

                fm1 = std::max(fm1, tw1);
                fm1 = std::max(fm1, tw2);
                fm2 = std::max(fm2, t1);
                fm2 = std::max(fm2, t2);
            }
        }

        s1 = ax*h*sum;
        double eps1 = fm1*1E-8;
        double eps2 = fm2*1E-8;
        double d1 = log10(std::abs(s1 - s2));
        double d2 = log10(std::abs(s1 - s3));
        double d3 = log10(eps1) - 1;
        double d4 = log10(eps2) - 1;

        if(k <= 2)
            err = 1.0;
        else if(d1 == -20.0)
            err = 0.0;
        else {
            double val1 = d1*d1/d2;
            double val2 = 2*d1;
            double max = std::max(val1, val2);
            max = std::max(max,d3);
            max = std::max(max,d4);
            err = pow(10.0, int(std::min(0.0,max)));
        }

        if(k > 3 && err < eps1) {
            delete ip;
            return s1;
        }

        if (k >= 3 && err < eps2) {
            std::cout << "Estimated error = 10^" << int(double(log(std::abs(err)))) << std::endl;
            delete ip;
            return s1;
        }
    }

    std::cout << "Estimated error = 10^" << int(double(log(std::abs(err)))) << std::endl;
    std::cout << "Increase _phases for greater accuracy." << std::endl;

    delete ip;
    return s1;
}

double testFunc1(double x) {
    return x*x+1;
}

double testFunc2(double x) {
    return sin(1.0/x);
}

double testFunc3(double x) {
    return exp(-x*x);
}

double testFunc4(double x) {
    return 1.0/(1+x*x);
}

double testFunc5(double x) {
    return 1.0/(2.0+cos(x));
}

double testFunc6(double x) {
    return j0(x);
}

int main(){
    std::function<double(double)> testFunc = testFunc1;
    std::cout << integrate(testFunc,0,4)-76.0/3.0 << std::endl;

    testFunc = testFunc2;
    std::cout << integrate(testFunc,0.1,2*M_PI)-2.271716601405714 << std::endl;

    testFunc = testFunc3;
    std::cout << integrate(testFunc,0,1)-0.746824132812427 << std::endl;

    testFunc = testFunc4;
    std::cout << integrate(testFunc,0,1)-M_PI_4 << std::endl;

    testFunc = testFunc5;
    std::cout << integrate(testFunc,0,M_PI_2)+integrate(testFunc,M_PI_2,M_PI) << std::endl;
}
