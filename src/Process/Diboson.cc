#include "../Process.hh"
#include "../Utility.hh"

#define NC 3.0
#define CA NC
#define CF (NC*NC-1)/(2*NC)
#define ZETA3 1.2020569032

// Hard part for Diphoton production
double AA::H(const double mu, const double s, const double t, const double u) {
    if(HOrder==0) return 1;
    else if(HOrder==1) return 1+pdf->Alpi(mu)*H1(mu,s,t,u);
    else if(HOrder==2) return 1+pdf->Alpi(mu)*(H1(mu,s,t,u)+pdf->Alpi(mu)*H2(mu,s,t,u));
    else throw std::runtime_error("Process: Invalid HOrder value");
}

double AA::H1(const double mu, const double s, const double t, const double u) {
    double v = -u/s;
    return CF/2.0*(M_PI*M_PI-7+1.0/(pow(1-v,2)+v*v)*((pow(1-v,2)+1)*pow(log(1-v),2)+v*(v+2)*log(1-v)+(v*v+1)*pow(log(v),2)+(1-v)*(3-v)*log(v)));
}

double AA::H2(const double mu, const double s, const double t, const double u) {
    double v = -u/s;
    int nf = pdf -> NFL(mu);

    return (F02(mu,s,t,u)+F11(mu,s,t,u))/ALO(v)+M_PI*M_PI/2.0*CF*H1(mu,s,t,u)
         - pow(M_PI,4)/8.0*CF*CF+CF*CA*(607.0/324.0+1181*M_PI*M_PI/864.0-187.0/144.0*ZETA3-7*pow(M_PI,4)/192.0)
         + CF*nf*(-41.0/162.0-97.0/432.0*M_PI*M_PI+17.0/72.0*ZETA3);
}

double AA::ALO(const double v) {
    return 8*NC*(1-2*v+2*v*v)/(v*(1-v));
}


// From hep-ph/0201274
double AA::F02(const double mu, const double s, const double t, const double u) {
    int nf = pdf -> NFL(s);
    double charge = 0;
    switch(nf) {
        case 6:
            charge += pow(2.0/3.0,2);
        case 5:
            charge += pow(1.0/3.0,2);
        case 4:
            charge += pow(2.0/3.0,2);
        default:
            charge += pow(2.0/3.0,2)+2.0*pow(1.0/3.0,2);
    }

    return 2*NC*(charge*nf/2.0*CF*(As(mu,s,t,u)+As(mu,s,u,t))+CF*CF*(Bs(mu,s,t,u)+Bs(mu,s,u,t))+CF*CA*(D2s(mu,s,t,u)+D2s(mu,s,u,t))
            +nf*CF*(E3s(mu,s,t,u)+E3s(mu,s,u,t)));
}

double AA::As(const double mu, const double s, const double t, const double u) {
    double x = -t/s, y = -u/s, z = -u/t;
    double X = log(x), Y = log(Y), S = log(s/(mu*mu)), U = log(-u/(mu*mu));

    return (128*xLi(4,z)-128*xLi(4,x)+128*xLi(4,y)+(-64.0/3.0+128*Y)*xLi(3,x)
        +  (64.0/3.0*X-64.0/3.0*M_PI*M_PI)*xLi(2,x)+16.0/3.0*pow(X,4)-64.0/3.0*pow(X,3)*Y+(-16+32.0/3.0*M_PI*M_PI+32*Y*Y)*X*X
        +  (-64.0/3.0*M_PI*M_PI*Y+48+160.0/9.0*M_PI*M_PI)*X+64.0/3.0*ZETA3+224.0/45.0*pow(M_PI,4)-128*Y*ZETA3)*t/u
        +  (32.0/3.0*xLi(3,x)-32.0/3.0*xLi(3,y)+(-32.0/3.0*X-32.0/3.0*Y)*xLi(2,x)
        +  (-32.0/3.0*Y*Y-80.0/9.0*M_PI*M_PI-64.0/3.0)*X+(64.0/3.0+32.0/3.0*M_PI*M_PI)*Y)*pow(t/s,2)
        + 24*pow(X*t/u,2)+(416.0/3.0*xLi(3,x)+64.0*xLi(3,y)*X-416.0/3.0*xLi(2,x)*X+(8*Y*Y+16)*X*X
        + (-8.0/3.0*Y+80.0/3.0+112.0/9.0*M_PI*M_PI-64.0*ZETA3-64*Y*Y)*X-416.0/3.0*ZETA3-148.0/9.0*M_PI*M_PI+44.0/45.0*pow(M_PI,4));
}

double AA::Bs(const double mu, const double s, const double t, const double u) {
    double x = -t/s, y = -u/s, z = -u/t;
    double X = log(x), Y = log(Y), S = log(s/(mu*mu)), U = log(-u/(mu*mu));

    return (-112*xLi(4,z)-88*xLi(4,y)+(-128*Y+48*X-64)*xLi(3,x)
         + (-16*Y-16*X+12)*xLi(3,y)+(12*Y-4*Y*Y+8*X*X-8*M_PI*M_PI+64*X)*xLi(2,x)
         + 2.0/3.0*pow(X,4)+56.0/3.0*pow(X,3)*Y+(44*Y-4*M_PI*M_PI+2-32*Y*Y)*X*X
         + (-4*pow(Y,3)-8-32*ZETA3-80.0/3.0*M_PI*M_PI+6*Y*Y+56.0/3.0*Y)*X
         + pow(Y,4)+6*pow(Y,3)+(-10.0/3.0*M_PI*M_PI-5)*Y*Y+(-39-18*M_PI*M_PI+144*ZETA3)*Y
         + 3*S+187.0/4.0-4.0*M_PI*M_PI*S+4.0/45.0*pow(M_PI,4)-5*M_PI*M_PI-20*ZETA3+48*ZETA3*S)*t/u
         + (-12*X*X+(24*Y+24)*X-12*Y*Y-24*Y-12*M_PI*M_PI)*pow(t/s,2)+8*pow(X*t/u,2)
         + (-80*xLi(4,y)+32*X*xLi(3,x)+(-128*X-152)*xLi(3,y)+152*xLi(2,x)*X
         + 8*Y*Y*xLi(2,y)+(-16*Y*Y-24)*X*X+(60*Y*Y+(28+32.0/3.0*M_PI*M_PI)*Y-58)*X
         + 14.0/3.0*pow(Y,4)+44.0/3.0*pow(Y,3)+8.0/3.0*pow(Y*M_PI,2)+(96*ZETA3-32.0/3.0*M_PI*M_PI)*Y+32.0/45.0*pow(M_PI,4)+16*ZETA3-86.0/3.0*M_PI*M_PI-2);
}

double AA::D2s(const double mu, const double s, const double t, const double u) {
    double x = -t/s, y = -u/s, z = -u/t;
    double X = log(x), Y = log(Y), S = log(s/(mu*mu)), U = log(-u/(mu*mu));

    return (48*xLi(4,z)-16*xLi(4,x)+24*xLi(4,y)+(56*Y-8*X+20)*xLi(3,x)
         + (8*X-12+16*Y)*xLi(3,y)+(16.0/3.0*M_PI*M_PI-20*X-12*Y-8*X*X+4*Y*Y)*xLi(2,x)
         + 1.0/3.0*pow(X,4)+(-8*Y-70.0/9.0)*pow(X,3)+(-4*M_PI*M_PI+286.0/9.0-16.0*Y+14*Y*Y-44.0/3.0*S)*X*X
         +(-22.0/9.0*M_PI*M_PI+4*pow(Y,3)-8*M_PI*M_PI*Y-6*Y*Y)*X-44.0/9.0*pow(Y,3)+(-4.0/3.0*M_PI*M_PI+35.0/9.0-22.0/3.0*S)*Y*Y
         +(57-26.0/9.0*M_PI*M_PI-72*ZETA3-22*S)*Y+479.0/9.0*ZETA3+19.0/60.0*pow(M_PI,4)-52*ZETA3*S+1141.0/27.0*S-215.0/18.0*M_PI*M_PI
         -43417.0/324.0+23.0/6.0*M_PI*M_PI*S)*t/u
         +(6*X*X+(-12-12*Y)*X+6*Y*Y+12*Y+6*M_PI*M_PI)*pow(t/s,2)-6*pow(X*t/u,2)
         +(16*xLi(4,y)+48*xLi(3,x)*Y+64*xLi(3,y)-8*Y*Y*xLi(2,y)-64*xLi(2,x)*X
         -4.0/3.0*pow(X,4)+(-20.0/3.0*M_PI*M_PI+6*Y*Y)*X*X+(-24*Y*Y+(-16.0/3.0*M_PI*M_PI-14)*Y-148.0/9.0*M_PI*M_PI)*X
         -112.0/9.0*pow(Y,3)+(-44.0/3.0*S+298.0/9.0)*Y*Y+(538.0/9.0-48.0*ZETA3-44.0/3.0*S)*Y-8*ZETA3-pow(M_PI,4)/3.0+61.0/9.0*M_PI*M_PI);
}

double AA::E3s(const double mu, const double s, const double t, const double u) {
    double x = -t/s, y = -u/s, z = -u/t;
    double X = log(x), Y = log(Y), S = log(s/(mu*mu)), U = log(-u/(mu*mu));

    return (16.0/9.0*pow(X,3)+(-76.0/9.0+8.0/3.0*S)*X*X+16.0/9.0*M_PI*M_PI*X
         + 8.0/9.0*pow(Y,3)+(4.0/3.0*S-2.0/9.0)*Y*Y+(8.0/9.0*M_PI*M_PI+4*S-10)*Y
         - 1.0/3.0*M_PI*M_PI*S-202.0/27.0*S+19.0/9.0*M_PI*M_PI-2.0/9.0*ZETA3+3401.0/162.0)*t/u
         + (16.0/9.0*M_PI*M_PI*X+16.0/9.0*pow(Y,3)+(8.0/3.0*S-52.0/9.0)*Y*Y
         + (-76.0/9.0+8.0/3.0*S)*Y+8.0/9.0*M_PI*M_PI);
}

double AA::F11(const double mu, const double s, const double t, const double u) {
    return NC*CF*CF*(G1s(mu,s,t,u)+G1s(mu,s,u,t));
}

double AA::G1s(const double mu, const double s, const double t, const double u) {
    double x = -t/s, y = -u/s, z = -u/t;
    double X = log(x), Y = log(Y), S = log(s/(mu*mu)), U = log(-u/(mu*mu));

    return (14*pow(X,4)+28*pow(X,3)+8*pow(X*Y,2)+56*pow(X*M_PI,2)-48*X*X+12*X*X*Y+32*X*Y*M_PI*M_PI+80*M_PI*M_PI*X
         + 2*pow(Y,4)+12*pow(Y,3)-10*pow(Y,2)+8*pow(Y*M_PI,2)+26*pow(M_PI,2)+24*M_PI*M_PI*Y-84.0*Y+102)*t/u
         + 8*X*(pow(X,3)+X*X+4*M_PI*M_PI*X+2*M_PI*M_PI)*pow(t/u,2)+2*X*X*(X*X+pow(2*M_PI,2))*pow(t/u,3)
         + (32*pow(X,3)+8*pow(X*Y,2)+80*M_PI*M_PI*X+32*X*Y*M_PI*M_PI+8*Y*Y*X
         + 8*pow(Y,4)+32*pow(Y*M_PI,2)-32*Y*Y-4-56*Y+24*M_PI*M_PI);
}
