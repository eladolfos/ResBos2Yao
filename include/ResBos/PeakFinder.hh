#ifndef PEAKFINDER_HH
#define PEAKFINDER_HH

#include <vector>
#include <map>
#include <cmath>

#include "ResBos/ResBos2.hh"
#include "ResBos/Resummation.hh"

class PeakFinder{
    public:
        PeakFinder(ResBos::ResBos* resum, double tol=1E-6) : resum(resum), tol(tol) {};
        void FindPeaks(double,double);
    private:
        double Q, y;
        double CalcRoot(double a, double b);
        inline double GetFunctionValue(double a) {
            double step = 0.01;
            std::vector<double> result1 = resum->GetCalc(Q,a-step,y);
            std::vector<double> result2 = resum->GetCalc(Q,a+step,y);
            return (result2[0]-result1[0])/(2*step);
        }
        inline double GetResummed(double a) {
            std::vector<double> result1 = resum->GetCalc(Q,a,y);
            return result1[0];
        }
        inline void swap(double &fa, double &fb, double &a, double &b) {
            if(std::abs(fa) < std::abs(fb)) {
                double tmp = a;
                a = b;
                b = tmp;
                tmp = fa;
                fa = fb;
                fb = tmp;
            }
            return;
        };
        ResBos::ResBos* resum;
        double tol;
        bool mflag;
};

#endif
