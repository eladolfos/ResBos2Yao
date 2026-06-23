#include "../Process.hh"
#include "../Utility.hh"

#define NC 3.0
#define CA NC
#define CF (NC*NC-1)/(2*NC)

// Add in the Sudakov factors that are only present with final state jets
void HJ::NonUniversal(int nf, std::map<std::string,double> &coef) {
    typedef PartType PT;
    switch(PType){
        case PT::gg:
            coef["D1"] = CA;
            break;
        case PT::qg:
            coef["D1"] = CF;
    }
}

// Calculate matrix element for Higgs + jet production
double HJ::ME(double *args, double *wgts) {
    // Higgs-Jet Tree level Matrix Element
    double Pj = args[0];
    double s = args[1];
    double t = args[2];
    double u = args[3];

    double SigS = wgts[0];
    double SigY = wgts[1];

    double H = H0(s,t,u);
    double K;

    typedef PartType PT;
    switch(PType){
        case PT::gg:
            K = 0.25/pow(NC*NC-1.0,2);
            break;
        case PT::qg:
            K = 0.25/(NC*NC-1.0)/NC;
            break;
    }

    return (wgts[0]+wgts[1])*pdf->Alpi(muR*mB)*M_PI/4.0/s*K*H;

}

// Calculate Hard part for Higgs + jet production
double HJ::HJet(const double mu, const double Pj, const double s, const double t, const double u) {
    int nf = pdf -> NFL(mu);
    if(HOrder==0) return 1;
    else if(HOrder==1) return 1+pdf->Alpi(mu)*H1(mu,Pj,s,t,u,nf);
    else throw std::runtime_error("Process: Invalid HOrder value");
}

double HJ::H0(const double s, const double t, const double u) {
    typedef PartType PT;
    switch(PType){
        case PT::gg:
            return CA*(CA*CA-1)*(pow(s,4)+pow(t,4)+pow(u,4)+pow(mB,8))/(s*t*u);
        case PT::qg:
            return -CA*CF/u*(pow(s,2)+pow(t,2));
    }
}

double HJ::H1(const double mu, const double Pj, const double s, const double t, const double u, const int nf) {
    double tb = mB*mB-t;
    double ub = mB*mB-u;

    double topCorrect = 11.0;
    double beta0 = (11*CA-2.0*nf)/12.0;
    double adTerm = beta0/CA*12.0*log(pow(muR*mB/mu,2))*3.0/2.0;
    double deltaH1;

    typedef PartType PT;
    switch(PType){
        case PT::gg:
            deltaH1 = pow(mB,2)*NC*(NC*NC-1)*(NC-nf)*(s*t*u+mB*mB*(s*t+s*u+t*u))/(3.0*s*t*u);
            return CA*(R+67.0/9.0-23*nf/54.0+pow(M_PI,2)/2.0+pow(log(tb/pow(mB,2)),2)-pow(log(-tb/t),2)-2*log(-t/s)*log(-u/s)+pow(log(ub/pow(mB,2)),2)
                    -pow(log(-ub/u),2)+log(pow(R,-2))*log(pow(muR*mB/Pj,2))+2*beta0/CA*log(pow(muR*mB/(Pj*R),2))-2*log(pow(muR*mB/Pj,2))*log(pow(muR*mB,2)/s)
                    +2*xLi(2,1-pow(mB,2)/s)+2*xLi(2,t/pow(mB,2))+2*xLi(2,u/pow(mB,2)))+deltaH1/H0(s,t,u)+topCorrect+adTerm+3.3;
        case PT::qg:
            deltaH1 = 2.0*(CA-CF)/2.0*CA*CF*(s+t);
            return 20*beta0/CA+CF*(R*-1.5-5*pow(M_PI,2)/6.0+pow(log(tb/pow(mB,2)),2)-pow(log(-tb/t),2)-log(pow(muR*mB/Pj,2))*log(pow(muR*mB,2)/s)
                    +3*log(-u/pow(muR*mB,2))-2*log(R)*log(pow(muR*mB/Pj,2))-log(u/t)*log(pow(Pj/(muR*mB),2))+pow(log(pow(muR*mB/Pj,2)),2)/2.0
                    +(3.0*log(pow(muR*mB/(Pj*R),2))/2.0+2*xLi(2,1-pow(mB,2)/s)+2*xLi(2,t/pow(mB,2))))
                    +CA*(7.0/3.0+4*pow(M_PI,2)/3.0+pow(log(ub/pow(mB,2)),2)-pow(log(-ub/u),2)+log(pow(Pj/(muR*mB),2))*log(pow(muR*mB,2)/s)
                    -4*beta0/CA*log(-u/pow(muR*mB,2))-2.0*log(-t/pow(muR*mB,2))*log(-u/pow(muR*mB,2))+log(u/t)*log(pow(Pj/(muR*mB),2))
                    +pow(log(pow(muR*mB/Pj,2)),2)/2.0+2*xLi(2,u/pow(mB,2)))+deltaH1/H0(s,t,u)+topCorrect+adTerm+3.3;
    }
}

// Calculate convolution piece for Higgs+jet production
double HJ::CxfCxf(double b, double x1, double x2, int mode) {
    mB = 125;
    // For testing only
    double gmu = 1.16637E-5;
    coupl = sqrt(2)*pow(pdf->Alpi(muR*mB),2)/(9.0)*gmu;
   

    double bs=bStar(b);
    double mu=C1/bs;
    int nfEff = pdf -> NFL(mu);

    typedef PartType PT;
    switch(PType){
        case PT::gg:
            return SearchCxFMap(0,x1,mu,nfEff)*SearchCxFMap(0,x2,mu,nfEff);
        case PT::qg:
            double gCxF = SearchCxFMap(0,x1,mu,nfEff);
            double qCxF = 0;
            for(int i = 1; i < 6; i++) {
                qCxF += SearchCxFMap(i,x2,mu,nfEff);
                qCxF += SearchCxFMap(-i,x2,mu,nfEff);
            }
            return gCxF*qCxF;
    }
}

// Calculate asymptotic piece for Higgs + jet production
std::vector<double> HJ::GetAsym(double x1, double x2, double amu, double Qt) {
    std::map<int,double> apdf1, apdf2;
    apdf1[0] = pdf->Apdf(0,x1,amu);
    apdf2[0] = pdf->Apdf(0,x2,amu);

    double g_g, g_q, q_g;
    std::vector<double> asym(nModesAsym);

    typedef PartType PT;
    switch(PType){
        case PT::gg:
            g_g = SplitSud(0,x1,amu,Qt)*apdf2[0]+apdf1[0]*SplitSud(0,x2,amu,Qt);

            g_q = 0, q_g = 0;

            for(int i = 1; i <= 5; i++){
                g_q += apdf1[0]*(SplitSud(i,x2,amu,Qt)+SplitSud(-i,x2,amu,Qt));
                q_g += apdf2[0]*(SplitSud(i,x1,amu,Qt)+SplitSud(-i,x1,amu,Qt));
            }

            asym[0] = g_g + g_q + q_g;

            return asym;
        case PT::qg:
            for(int i = 1; i <= 5; i++){
                g_q += apdf1[0]*(SplitSud(i,x2,amu,Qt)+SplitSud(-i,x2,amu,Qt));
                q_g += apdf2[0]*(SplitSud(i,x1,amu,Qt)+SplitSud(-i,x1,amu,Qt));
            }
    }
    
}
