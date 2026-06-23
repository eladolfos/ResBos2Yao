#include "ResBos/Brent.hh"
#include "ResBos/loguru.hpp"

#include <iostream>

constexpr double Brent::tiny;
constexpr double Brent::glimit;
constexpr double Brent::gold;
constexpr double Brent::cgold;
constexpr double Brent::eps;

// Use the Brent algorithm to calculate the root of a given function, between x=a and x=b
double Brent::CalcRoot(double a, double b) {
    double fa = func(a), fb = func(b), fc;

    if(fa*fb >= 0) throw std::runtime_error("No root in given range");
    swap(fa,fb,a,b);
    double c = a;
    mflag = true;
    double s, d =0;
    while(fb != 0 && std::abs(b-a) > tol) {
        fc = func(c);
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
        double fs = func(s);
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

void Brent::GetBracket(double &a, double &b, double &c,
                       double &fa, double &fb, double &fc) const {
    fa = func(a);
    fb = func(b);

    // Ensure we are going downhill
    swap(fa, fb, a, b);

    // First guess for c
    c = b+gold*(b-a);
    fc = func(c);
    double u, fu;

    // Keep updating guess until we bracket, start with parabolic extrapolation
    while (fb > fc) {
        double r = (b-a)*(fb-fc);
        double q = (b-c)*(fb-fa);
        // tiny is used to prevent division by zero
        u = b-((b-c)*q-(b-a)*r)/(2.0*std::max(std::abs(q-r), tiny)*std::signbit(q-r));
        double ulim = b+glimit*(c-b);
        // Parabolic u is between b and c. Try it for the bracket
        if((b-u)*(u-c) > 0.0) {
            fu=func(u);
            // Minimum between b and c
            if(fu < fc) {
                a = b;
                b = u;
                fa = fb;
                fb = fu;

                return;
            // Minimum between a and u
            } else if(fu > fb) {
                c = u;
                fc = fu;

                return;
            }
            // Parabolic fit failed, use default magnifiction
            u = c + gold*(c-b);
            fu = func(u);
        // Parabolic fit is between c and its allowed limit
        } else if((c-u)*(u-ulim) > 0.0) {
            fu = func(u);
            if(fu < fc) {
                shift(b, c, u, c+gold*(c-b));
                shift(fb, fc, fu, func(u));
            }
        // Limit parabolic u to maximum allowed value
        } else if((u-ulim)*(ulim-c) >= 0.0) {
            u = ulim;
            fu = func(u);
        // Reject parabolic u, use default magnification
        } else {
            u = c+gold*(c-b);
            fu = func(u);
        }
        // Eliminate oldest point and continue
        shift(a, b, c, u);
        shift(fa, fb, fc, fu);
    }
}

double Brent::Minimize(const double &ax, const double &bx, const double &cx) const {
    double a = ax, b = bx, c = cx;
    double fa, fb, fc;
    if(c > 1E98) {
        GetBracket(a, b, c, fa, fb, fc);
    }

    double d = 0.0, e = 0.0;
    double x, w, v, u;
    double fw, fv, fx, fu;

    // Initialize
    x=w=v=b;
    fw=fv=fx=fb;
    for(size_t it = 0; it < itmax; ++it) {
        double xm = 0.5*(a+b);
        double tol1 = tol*std::abs(x)+eps;
        double tol2 = 4.0*tol1;

        // Test for minimum
        if(std::abs(x-xm) <= (tol2-0.5*(b-a))) {
            return x;
        }

        // Construct a trial parabolic fit
        if(std::abs(e) > tol1) {
            double r = (x-w)*(fx-fv);
            double q = (x-v)*(fx-fw);
            double p = (x-v)*q-(x-w)*r;
            q=2.0*(q-r);
            if(q > 0.0) p = -p;
            q=std::abs(q);
            double etemp = e;
            e = d;

            // Determine acceptability of the parabolic fit, if fail use golden section
            if(std::abs(p) >= std::abs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x)) {
                e = x >= xm ? a-x : b-x;
                d=cgold*e;
            // Use the parabolic step
            } else {
                d = p/q;
                u = x+d;
                if(u-a < tol2 || b-u < tol2) d=tol1*std::signbit(xm-x);
            }
        } else {
            e = x >= xm ? a-x : b-x;
            d = cgold*e;
        }
        u = std::abs(d) >= tol1 ? x+d : x+tol1*std::signbit(d);
        fu = func(u); // Only function evaluation per iteration
        if(fu <= fx) {
            if(u >= x) a=x;
            else b=x;
            shift(v, w, x, u);
            shift(fv, fw, fx, fu);
        } else {
            if(u < x) a=u;
            else b=u;
            if(fu <= fw || w == x) {
                v=w;
                w=u;
                fv=fw;
                fw=fu;
            } else if(fu <= fv || v == x || v == w) {
                v=u;
                fv=fu;
            }
        }
    }

    DLOG_F(WARNING, "Too many Brent iterations.");
    return x;
}
