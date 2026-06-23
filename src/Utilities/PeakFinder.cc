#include "ResBos/PeakFinder.hh"

#include <iostream>

double PeakFinder::CalcRoot(double a, double b) {
    double fa = GetFunctionValue(a), fb = GetFunctionValue(b), fc;
    if(fa*fb > 0) throw std::runtime_error("No root in given range");
    swap(fa,fb,a,b);
    double c = a;
    mflag = true;
    double s, d =0;
    int i = 0;
    while(fb != 0 && std::abs(b-a) > tol) {
        fc = GetFunctionValue(c);
        if(fa != fc && fb != fc) {
            s = (a*fb*fc)/((fa-fb)*(fa-fc))+(b*fa*fc)/((fb-fa)*(fb-fc))+(c*fa*fb)/((fc-fa)*(fc-fb));
        } else {
            s = b-fb*(b-a)/(fb-fa);
        }
        if((s > std::max((3.0*a+b)/4.0,b) || s < std::min((3.0*a+b)/4.0,b)) ||
                (mflag && std::abs(s-b) >= std::abs(b-c)/2.0) ||
                (!mflag && std::abs(s-b) >= std::abs(c-d)/2.0) ||
                (mflag && std::abs(b-c) < tol) ||
                (!mflag && std::abs(c-d) < tol)) {
            s = (a+b)/2.0;
            mflag = true;
        } else {
            mflag = false;
        }
        double fs = GetFunctionValue(s);
        d = c;
        c = b;
        if(fa*fs < 0) {
            b = s;
            fb = fs;
        } else {
            a = s;
            fa = fs;
        }
        swap(fa,fb,a,b);
    }

    return b;
}

void PeakFinder::FindPeaks(double qMin, double qMax) {
    Q = qMin;
    y = 0;
    while(Q < qMax) {
        double result = CalcRoot(2,10);
        std::cout << Q << "  " << result << "  " << exp(result) << "  " << GetResummed(result-0.01) << "  " << GetResummed(result) << "  " << GetResummed(result+0.01) << std::endl;
        Q += 10;
    }
}
