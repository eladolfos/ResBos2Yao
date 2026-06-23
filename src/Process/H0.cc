#include "../Process.hh"
#include <complex>

// Test matrix element for higgs
double H0::METest() {
    return H(125);
}

// Currently only for the infinite mass limit
double H0::Ch(const double mQ) {
    return 11.0/2.0;
}

// Hard part for Higgs Production
double H0::H(const double mu) {
    int nf = pdf -> NFL(mu);
    if(HOrder==0) return 1;
    else if(HOrder==1) return 1+pdf->Alpi(mu)*H1(172);
    else if(HOrder==2) return 1+pdf->Alpi(mu)*(H1(172)+pdf->Alpi(mu)*H2(nf,172));
    else throw std::runtime_error("Process: Invalid HOrder value");
}

double H0::H1() {
    return H1(mt);
}

double H0::H1(const double mQ) {
    return CA*M_PI*M_PI/2.0+Ch(mQ);
}

double H0::H2(const int nf, const double mQ) {
    double Lq = log(pow(mh/mQ,2));
    return CA*CA*(3187.0/288.0+7.0/8.0*Lq+157.0/72.0*M_PI*M_PI+13.0/144.0*pow(M_PI,4)-55.0/18.0*ZETA3)
         + CA*CF*(-145.0/24.0-11.0/8.0*Lq-3.0/4.0*M_PI*M_PI)+9.0/4.0*CF*CF-5.0/96.0*CA
         - CA*nf*(287.0/144.0+5.0/36.0*M_PI*M_PI+4.0/9.0*ZETA3)
         + CF*nf*(-41.0/24.0+0.5*Lq+ZETA3);
}

double H0::Enfs(double Q){
    std::complex<double> I = (0,1), A=0, f;
    double t;
    for(int i = 0; i < 3; i++) {
        if(i==0) t = pow(Q/mc,2)/4.0;
        else if(i==1) t = pow(Q/mb,2)/4.0;
        else t = pow(Q/mt,2)/4.0;
        if(t <= 1) f = pow(asin(sqrt(t)),2);
        else {
            double t1 = sqrt(1-1.0/t);
            double t2 = 1.0 + t1;
            double t3 = 1.0 - t1;
            f = -1.0/4.0*pow(log(t2/t3)-I*M_PI,2);
        }
        A+= 2.0*(t+(t-1.0)*f)/pow(t,2);
    }

    return norm(3.0/4.0*A);
}

// Calculation of Higgs Matrix Element
double H0::ME(double *args, double *wgts) {
    // This section is for testing purposes only
    mc = 1.3;
    mb = 4.5;
    mt = 172;
    gamh = 0.004;
    
    //Match variables to that in the original ResBos code for convience of checking
    double Q = args[0];
    double SigS = wgts[0], SigY = wgts[1];

    double Prop;
    if(mh == 0) Prop = pow(Q,4);
    else Prop = pow(Q*Q-mh*mh,2)+pow(Q,4)*pow(gamh/mh,2);

    return Enfs(Q)*(SigS+SigY)/Prop*32.0*M_PI*Q*gamh*2.0*Q*Q/2;
}

// Calculate the asymptotic piece for the Higgs boson
std::vector<double> H0::GetAsym(double x1, double x2, double amu, double Qt){
    std::map<int,double> apdf1, apdf2;

    apdf1[0] = pdf->Apdf(0,x1,amu);
    apdf2[0] = pdf->Apdf(0,x2,amu);

    double g_g = SplitSud(0,x1,amu,Qt)*apdf2[0]+apdf1[0]*SplitSud(0,x2,amu,Qt);

    double g_q = 0, q_g = 0;

    for(int i = 1; i <= 5; i++){
        g_q += apdf1[0]*(SplitSud(i,x2,amu,Qt)+SplitSud(-i,x2,amu,Qt));
        q_g += apdf2[0]*(SplitSud(i,x1,amu,Qt)+SplitSud(-i,x1,amu,Qt));
    }

    std::vector<double> asym(1);
    asym[0] = g_g + g_q + q_g;

    return asym;
}

// Calculate the convolution piece for the Higgs boson
double H0::CxfCxf(double b, double x1, double x2, int mode) {
    mh = 125;

    double zBeta0 = 4.0*(11*3-10)/12.0;
    double zBeta1 = 34*3-10*(4.0/3.0+5);
    double gmu = 1.16637E-5;
    double hcoupl = sqrt(2.0)*gmu*M_PI/576.0;
    coupl = hcoupl*pow(pdf -> Alpi(mh)*mh,2);

    double bs=bStar(b);
    double mu=C1/bs;
    int nfEff = pdf -> NFL(mu);

    return SearchCxFMap(0,x1,mu,nfEff)*SearchCxFMap(0,x2,mu,nfEff);
}

// Calculate the perturbative piece for the Higgs boson
std::vector<double> H0::PLumi1(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    std::vector<double> pLumi(1);

    double g_g = pdf->Apdf(0,x1,amu)*pdf->Apdf(0,x2,amu);
    
    pLumi[0] = 1.0/sh*g_g*Hamp2GG(sh,th,uh,Q,Qt);

    return pLumi;
}

// Calculate the perturbative piece for the Higgs boson
std::vector<double> H0::PLumi2(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    std::vector<double> pLumi(1);
    std::map<int,double> apdf1, apdf2;

    apdf1[0] = pdf->Apdf(0,x1,amu);
    apdf2[0] = pdf->Apdf(0,x2,amu);

    double g_q = 0, q_g = 0, q_qb = 0, qb_q;

    for(int i = 1; i <= 5; i++){
        g_q += apdf1[0]*(pdf->Apdf(i,x2,amu)+pdf->Apdf(-i,x2,amu));
        q_g += apdf2[0]*(pdf->Apdf(i,x1,amu)+pdf->Apdf(-i,x1,amu));
        q_qb += pdf->Apdf(i,x1,amu)*pdf->Apdf(-i,x2,amu);
        qb_q += pdf->Apdf(-i,x1,amu)*pdf->Apdf(i,x2,amu);
    }

    pLumi[0] = 1.0/sh*(g_q*Hamp2GQ(sh,th,uh,Q,Qt)+q_g*Hamp2GQ(sh,uh,th,Q,Qt));
    pLumi[0] += 1.0/sh*(q_qb+qb_q)*Hamp2QQ(sh,th,uh,Q,Qt);

    return pLumi;

}

double H0::Hamp2GG(const double sh, const double th, const double uh, const double Q, const double Qt) {
    return 3.0/2.0/M_PI/(Q*Q)*(pow(Q,8)+pow(sh,4)+pow(th,4)+pow(uh,4))/sh/th/uh;
}

double H0::Hamp2GQ(const double sh, const double th, const double uh, const double Q, const double Qt) {
    return -2.0/3.0/M_PI/(Q*Q)*(pow(sh,2)+pow(uh,2))/th;
}

double H0::Hamp2QQ(const double sh, const double th, const double uh, const double Q, const double Qt) {
    return 16.0/9.0/M_PI/(Q*Q)*(pow(th,2)+pow(uh,2))/sh;
}
