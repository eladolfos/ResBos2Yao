#include "../Process.hh"
#include "../Utility.hh"

#define NC 3.0
#define CA NC
#define CF (NC*NC-1)/(2*NC)

// Function to set the pieces that only occur with final state colored particles
void Wtb::NonUniversal(int nf, std::map<std::string,double> &coef) {
    coef["D1"] = -CF/2.0;
    coef["M1"] = CF/2.0;
}

// Calculate the matrix element for W->tb
double Wtb::ME(double *args, double *wgts) {
    double s = args[0];
    double t = args[1];
    double u = args[2];

    double SigS = wgts[0];
    double SigY = wgts[1];
    double Rt=pow(g2,2)*Vi*Vi*Vf*Vf/8.0;

    double H0 = 2*CA/3.0*Rt*t*(t-mt*mt)/(pow(s-mw*mw,2)+pow(mw,2)*GammaW2);

    return H0/16.0/s;
}

// Calculate the hard part for W->tb
double Wtb::HJet(const double mu, const double Pj, const double s, const double t, const double u) {
    int nf = pdf -> NFL(mu);
    if(HOrder==0) return 1;
    else if(HOrder==1) return 1+pdf->Alpi(mu)*H1(s,t,u,Pj,mu);
    else throw std::runtime_error("Process: Invalid HOrder value");
}

double Wtb::H1(double s, double t, double u, double Pj, double mu) {
    double mu2 = mu*mu;
    double PJ2 = Pj*Pj;
    double mt2 = mt*mt;
    double theta=acos((u-t)/(s-mt2));
    double eta=1;
    double r2=s/mt2;
    double LnMu2Mt2=log(mu2/mt2); 
    double LnR2M1=log(r2-1);
    double pi = M_PI;

    return CF*(-1/2.0*pow(LnMu2Mt2,2)-(5.0/2.0-2.0*LnR2M1)*LnMu2Mt2-(11.0+eta)/2.0-pi*pi/6.0-2.0*xLi(2,r2)
            +3.0*LnR2M1-2.0*pow(LnR2M1,2)-1.0/r2-1.0/r2*LnR2M1+1.0/r2*(1-r2)/(1+2.0*r2)*LnR2M1
            +1.0/2.0*pow(log(PJ2*R*R/mu2),2)-3.0/2.0*log(PJ2*R*R/mu2)+13.0/2.0-3.0/2.0*pi*pi
            +4.0*log(s/mt2)*log(1.0/cos(theta/2.0))-xLi(2,-mt2/s*tan(theta/2.0)*tan(theta/2.0))+xLi(2,-s/mt2*pow(tan(theta/2.0),2))
            -1.0/2.0*log(1.0/R/R)-1.0/2.0*log(1/cos(theta/2.0)/cos(theta/2.0))*log(1/pow(sin(theta/2.0),2))
            -pow(log(mu2/s),2)-3.0*log(mu2/s)-8+pi*pi);
}

// Calculate the convolution for W->tb
double Wtb::CxfCxf(double b, double x1, double x2, int mode) {
    double bs=bStar(b);
    double mu=C1/bs;
    int nfEff = pdf -> NFL(mu);

    std::map<int,double> convPart1, convPart2;
    convPart1[-2] = SearchCxFMap(-2,x1,mu,nfEff);
    convPart1[-4] = SearchCxFMap(-4,x1,mu,nfEff);
    convPart2[1] = SearchCxFMap(1,x2,mu,nfEff);
    convPart2[3] = SearchCxFMap(3,x2,mu,nfEff);
    convPart2[5] = SearchCxFMap(5,x2,mu,nfEff);

    double ub_d = VKM.GetCKM(0,0)*convPart1[-2]*convPart2[1]
                + VKM.GetCKM(0,1)*convPart1[-2]*convPart2[3]
                + VKM.GetCKM(0,2)*convPart1[-2]*convPart2[5]
                + VKM.GetCKM(1,0)*convPart1[-4]*convPart2[1]
                + VKM.GetCKM(1,1)*convPart1[-4]*convPart2[3]
                + VKM.GetCKM(1,2)*convPart1[-4]*convPart2[5];

    convPart2[-2] = SearchCxFMap(-2,x2,mu,nfEff);
    convPart2[-4] = SearchCxFMap(-4,x2,mu,nfEff);
    convPart1[1] = SearchCxFMap(1,x1,mu,nfEff);
    convPart1[3] = SearchCxFMap(3,x1,mu,nfEff);
    convPart1[5] = SearchCxFMap(5,x1,mu,nfEff);

    double d_ub = VKM.GetCKM(0,0)*convPart2[-2]*convPart1[1]
                + VKM.GetCKM(0,1)*convPart2[-2]*convPart1[3]
                + VKM.GetCKM(0,2)*convPart2[-2]*convPart1[5]
                + VKM.GetCKM(1,0)*convPart2[-4]*convPart1[1]
                + VKM.GetCKM(1,1)*convPart2[-4]*convPart1[3]
                + VKM.GetCKM(1,2)*convPart2[-4]*convPart1[5];

    if(mode == 0) return d_ub+ub_d;
    else return d_ub-ub_d;
}

// Calculate the asymptotic piece for W->tb
std::vector<double> Wtb::GetAsym(double x1, double x2, double amu, double Qt){
    std::map<int,double> splitSud1, splitSud2;
    std::map<int,double> apdf1, apdf2;

    apdf2[1] = pdf -> Apdf(1,x2,amu);
    apdf2[3] = pdf -> Apdf(3,x2,amu);
    apdf2[5] = pdf -> Apdf(5,x2,amu);
    splitSud1[-2] = SplitSud(-2,x1,amu,Qt);
    splitSud1[-4] = SplitSud(-4,x1,amu,Qt);

    apdf1[1] = pdf -> Apdf(1,x1,amu);
    apdf1[3] = pdf -> Apdf(3,x1,amu);
    apdf1[5] = pdf -> Apdf(5,x1,amu);
    splitSud2[-2] = SplitSud(-2,x2,amu,Qt);
    splitSud2[-4] = SplitSud(-4,x2,amu,Qt);

    apdf1[-2] = pdf -> Apdf(-2,x1,amu);
    apdf1[-4] = pdf -> Apdf(-4,x1,amu);
    splitSud2[1] = SplitSud(1,x2,amu,Qt);
    splitSud2[3] = SplitSud(3,x2,amu,Qt);
    splitSud2[5] = SplitSud(5,x2,amu,Qt);

    apdf2[-2] = pdf -> Apdf(-2,x2,amu);
    apdf2[-4] = pdf -> Apdf(-4,x2,amu);
    splitSud1[1] = SplitSud(1,x1,amu,Qt);
    splitSud1[3] = SplitSud(3,x1,amu,Qt);
    splitSud1[5] = SplitSud(5,x1,amu,Qt);

    double d_ub = VKM.GetCKM(0,0)*splitSud2[-2]*apdf1[1]
                + VKM.GetCKM(0,1)*splitSud2[-2]*apdf1[3]
                + VKM.GetCKM(0,2)*splitSud2[-2]*apdf1[5]
                + VKM.GetCKM(1,0)*splitSud2[-4]*apdf1[1]
                + VKM.GetCKM(1,1)*splitSud2[-4]*apdf1[3]
                + VKM.GetCKM(1,2)*splitSud2[-4]*apdf1[5];

    double ub_d = VKM.GetCKM(0,0)*splitSud1[-2]*apdf2[1]
                + VKM.GetCKM(0,1)*splitSud1[-2]*apdf2[3]
                + VKM.GetCKM(0,2)*splitSud1[-2]*apdf2[5]
                + VKM.GetCKM(1,0)*splitSud1[-4]*apdf2[1]
                + VKM.GetCKM(1,1)*splitSud1[-4]*apdf2[3]
                + VKM.GetCKM(1,2)*splitSud1[-4]*apdf2[5];
        
    double d_ub_2 = VKM.GetCKM(0,0)*apdf2[-2]*splitSud1[1]
                  + VKM.GetCKM(0,1)*apdf2[-2]*splitSud1[3]
                  + VKM.GetCKM(0,2)*apdf2[-2]*splitSud1[5]
                  + VKM.GetCKM(1,0)*apdf2[-4]*splitSud1[1]
                  + VKM.GetCKM(1,1)*apdf2[-4]*splitSud1[3]
                  + VKM.GetCKM(1,2)*apdf2[-4]*splitSud1[5];

    double ub_d_2 = VKM.GetCKM(0,0)*apdf1[-2]*splitSud2[1]
                  + VKM.GetCKM(0,1)*apdf1[-2]*splitSud2[3]
                  + VKM.GetCKM(0,2)*apdf1[-2]*splitSud2[5]
                  + VKM.GetCKM(1,0)*apdf1[-4]*splitSud2[1]
                  + VKM.GetCKM(1,1)*apdf1[-4]*splitSud2[3]
                  + VKM.GetCKM(1,2)*apdf1[-4]*splitSud2[5];

    double asym_1 = d_ub+d_ub_2+ub_d+ub_d_2;
    double asym_2 = d_ub+d_ub_2-ub_d-ub_d_2;

    double g_ub = SplitSud(0,x1,amu,Qt)*(
                 apdf2[-2]*(VKM.GetCKM(0,0)+VKM.GetCKM(0,1)+VKM.GetCKM(0,2))
               + apdf2[-4]*(VKM.GetCKM(1,0)+VKM.GetCKM(1,1)+VKM.GetCKM(1,2)) );

    double ub_g = SplitSud(0,x2,amu,Qt)*(
                 apdf1[-2]*(VKM.GetCKM(0,0)+VKM.GetCKM(0,1)+VKM.GetCKM(0,2))
               + apdf1[-4]*(VKM.GetCKM(1,0)+VKM.GetCKM(1,1)+VKM.GetCKM(1,2)) );

    double g_d = SplitSud(0,x1,amu,Qt)*(
                  apdf2[1]*(VKM.GetCKM(0,0)+VKM.GetCKM(1,0))
                + apdf2[3]*(VKM.GetCKM(0,1)+VKM.GetCKM(1,1))
                + apdf2[5]*(VKM.GetCKM(0,2)+VKM.GetCKM(1,2)) );

    double d_g = SplitSud(0,x2,amu,Qt)*(
                  apdf1[1]*(VKM.GetCKM(0,0)+VKM.GetCKM(1,0))
                + apdf1[3]*(VKM.GetCKM(0,1)+VKM.GetCKM(1,1))
                + apdf1[5]*(VKM.GetCKM(0,2)+VKM.GetCKM(1,2)) );
                 

    double asym_3 = g_ub + ub_g + d_g + g_d;
    double asym_4 = g_ub - ub_g + d_g - g_d;

    std::vector<double> asym(2);
    asym[0] = asym_1+asym_3;
    asym[1] = asym_2+asym_4;

    return asym;
}

// Helper function to calculate (p_1+p_2)^2
inline double sij(const TLorentzVector p1, const TLorentzVector p2){
    return (p1[3]+p2[3])*(p1[3]+p2[3])-(p1[1]+p2[1])*(p1[1]+p2[1])-(p1[2]+p2[2])*(p1[2]+p2[2])-(p1[0]+p2[0])*(p1[0]+p2[0]);
}

// Helper function to calculate (p_1-p_2)^2
inline double tij(const TLorentzVector p1, const TLorentzVector p2){
    return (p1[3]-p2[3])*(p1[3]-p2[3])-(p1[1]-p2[1])*(p1[1]-p2[1])-(p1[2]-p2[2])*(p1[2]-p2[2])-(p1[0]-p2[0])*(p1[0]-p2[0]);
}

// Real correction matrix element for W->tb with qq initial state
double Wtb::M2qq(std::map<int,TLorentzVector> pLabIn, std::map<int,TLorentzVector> pLabOut, double scale){
    TLorentzVector p1 = pLabIn[1], p2 = pLabIn[2], lBottom = pLabOut[1], lTop = pLabOut[2], lJet = pLabOut[3];
    double mt2=mt*mt;
    double s12=sij(p1,p2);
    double s34=sij(lBottom,lTop);
    double s35=sij(lBottom,lJet);
    double s45=sij(lTop,lJet);

    double t24=tij(p2,lTop);
    double t13=tij(p1,lBottom);
    double t15=tij(p1,lJet);
    double t25=tij(p2,lJet);
    double t14=tij(p1,lTop);
    double t23=tij(p2,lBottom);

    double s12p=s12-mt2;
    double s34p=s34-mt2;
    double s35p=s35-mt2;
    double s45p=s45-mt2;

    double t24p=t24-mt2;
    double t13p=t13-mt2;
    double t15p=t15-mt2;
    double t25p=t25-mt2;
    double t14p=t14-mt2;
    double t23p=t23-mt2;

    double Rt=pow(g2,2)*Vi*Vi*Vf*Vf/8.0;

    double prop1=pow(s34-mw2,2)+mw2*GammaW2;
    double Mat1=-(t24p*(t13+s35)-t13*t14p)/t15-(t13*(t24p+s45p)-t23*t24p)/t25-s12*(t13*(2*t24p+s45p)+s35*t24p)/t15/t25;

    double prop2=pow(s12-mw2,2)+mw2*GammaW2;
    double Mat2=-(t24p*(t13+t15)-t13*t23)/s35-(t13*(t24p+t25)*(1-2*mt2/s45p)-t14p*t24p)/s45p-s34p*(t13*(2*t24p+t25)+t15*t24p)/s35/s45p;

    double coeff=-8*M_PI*pdf->Alphas(scale)*2*CF*Rt;

    return coeff*(Mat1/prop1+Mat2/prop2);
}

// Real correction matrix element for W->tb with qg initial state
double Wtb::M2qg(std::map<int,TLorentzVector> pLabIn, std::map<int,TLorentzVector> pLabOut, double scale){
    TLorentzVector p1 = pLabIn[1], p2 = pLabIn[2], lBottom = pLabOut[1], lTop = pLabOut[2], lJet = pLabOut[3];
    double mt2=mt*mt;
    double s12=sij(p1,p2);
    double s34=sij(lBottom,lTop);
    double s35=sij(lBottom,lJet);
    double s45=sij(lTop,lJet);

    double t24=tij(p2,lTop);
    double t13=tij(p1,lBottom);
    double t15=tij(p1,lJet);
    double t25=tij(p2,lJet);
    double t14=tij(p1,lTop);
    double t23=tij(p2,lBottom);

    double s12p=s12-mt2;
    double s34p=s34-mt2;
    double s35p=s35-mt2;
    double s45p=s45-mt2;

    double t24p=t24-mt2;
    double t13p=t13-mt2;
    double t15p=t15-mt2;
    double t25p=t25-mt2;
    double t14p=t14-mt2;
    double t23p=t23-mt2;

    double Rt=pow(g2,2)*Vi*Vi*Vf*Vf/8.0;

    double prop=pow(s34-mw2,2)+mw2*GammaW2;
    double Mat=(s45p*(t13+t23)-t13*t14p)/s12+(t13*(s45p+t24p)-s35*s45p)/t25+(t15*(s45p*(2*t13+t23)+t13*t24p))/(s12*t25);

    double coeff=-8*M_PI*pdf->Alphas(scale)*Rt;

    return coeff*Mat/prop;
}

// Calculate the perturbative piece for W->tb
std::vector<double> Wtb::Pert(std::vector<double> Point) {
    std::map<int,TLorentzVector> pLabIn;
    std::map<int,TLorentzVector> pLabOut;

    TLorentzVector lTop, lBottom, lJet, lW;
    double E1 = sqrt(mt*mt+(pow(Point[0],2)+pow(Point[1],2)+pow(Point[2],2)));
    double E2 = sqrt((pow(Point[4],2)+pow(Point[5],2)+pow(Point[6],2)));
    double x1 = Point[7]/ECM;
    double x2 = Point[8]/ECM;

    pLabIn[1].SetPxPyPzE(0,0,Point[6],Point[6]);
    pLabIn[2].SetPxPyPzE(0,0,Point[7],-Point[7]);

    lTop.SetPxPyPzE(Point[0],Point[1],Point[2],E1);
    lBottom.SetPxPyPzE(Point[3],Point[4],Point[5],E2);
    lW = lTop+lBottom;
    lJet.SetPxPyPzE(-lW.Px(),-lW.Py(),Point[6]-Point[7]+lW.Pz(),Point[6]+Point[7]+lW.E());

    pLabOut[1] = lBottom;
    pLabOut[2] = lTop;
    pLabOut[3] = lJet;

    std::vector<double> result = {0};

    if(lBottom.Pt() == 0 || lJet.Pt() == 0 || lTop.Pt() == 0) return result;
    if(lBottom.Pt() != lBottom.Pt() || lJet.Pt() != lJet.Pt() || lTop.Pt() != lTop.Pt()) return result;

    double DeltaR=lBottom.DeltaR(lJet);
    if(DeltaR < R) return result;

    double qtt;
    double pj;

    double scale;

    if(lBottom.Pt() >= lJet.Pt() && std::abs(lBottom.Rapidity()) <= jetYCut) {
        pj = lBottom.Pt();
        qtt = lJet.Pt();
        scale = sij(lTop,lBottom);
    } else if(lBottom.Pt() <= lJet.Pt() && std::abs(lJet.Rapidity()) <= jetYCut) {
        pj = lTop.Pt();
        qtt = lBottom.Pt();
        scale = sij(lTop,lJet);
    } else if(std::abs(lBottom.Rapidity()) <= jetYCut && std::abs(lJet.Rapidity()) > jetYCut) {
        pj = lBottom.Pt();
        qtt = lJet.Pt();
        scale = sij(lTop,lBottom);
    } else if(std::abs(lJet.Rapidity()) <= jetYCut && std::abs(lBottom.Rapidity()) > jetYCut) {
        pj = lTop.Pt();
        qtt = lBottom.Pt();
        scale = sij(lTop,lJet);
    } else return result;

    scale = sqrt(scale);

    double amu = C4*lW.M();

    std::map<int,double> apdf1, apdf2;
    apdf1[2] = pdf -> Apdf(2,x1,amu);
    apdf1[4] = pdf -> Apdf(4,x1,amu);
    apdf2[-1] = pdf -> Apdf(-1,x2,amu);
    apdf2[-3] = pdf -> Apdf(-3,x2,amu);
    apdf2[-5] = pdf -> Apdf(-5,x2,amu);
    apdf2[2] = pdf -> Apdf(2,x2,amu);
    apdf2[4] = pdf -> Apdf(4,x2,amu);
    apdf1[-1] = pdf -> Apdf(-1,x1,amu);
    apdf1[-3] = pdf -> Apdf(-3,x1,amu);
    apdf1[-5] = pdf -> Apdf(-5,x1,amu);

    double u_db = VKM.GetCKM(0,0)*apdf1[2]*apdf2[-1] 
                + VKM.GetCKM(0,1)*apdf1[2]*apdf2[-3] 
                + VKM.GetCKM(0,2)*apdf1[2]*apdf2[-5] 
                + VKM.GetCKM(1,0)*apdf1[4]*apdf2[-1] 
                + VKM.GetCKM(1,1)*apdf1[4]*apdf2[-3] 
                + VKM.GetCKM(1,2)*apdf1[4]*apdf2[-5];

    double db_u = VKM.GetCKM(0,0)*apdf2[2]*apdf1[-1] 
                + VKM.GetCKM(0,1)*apdf2[2]*apdf1[-3] 
                + VKM.GetCKM(0,2)*apdf2[2]*apdf1[-5] 
                + VKM.GetCKM(1,0)*apdf2[4]*apdf1[-1] 
                + VKM.GetCKM(1,1)*apdf2[4]*apdf1[-3] 
                + VKM.GetCKM(1,2)*apdf2[4]*apdf1[-5];

    double g_u = pdf -> Apdf(0,x1,amu)*(
                 pdf -> Apdf(2,x2,amu)*(VKM.GetCKM(0,0)+VKM.GetCKM(0,1)+VKM.GetCKM(0,2))
               + pdf -> Apdf(4,x2,amu)*(VKM.GetCKM(1,0)+VKM.GetCKM(1,1)+VKM.GetCKM(1,2)) );

    double u_g = pdf -> Apdf(0,x2,amu)*(
                 pdf -> Apdf(2,x1,amu)*(VKM.GetCKM(0,0)+VKM.GetCKM(0,1)+VKM.GetCKM(0,2))
               + pdf -> Apdf(4,x1,amu)*(VKM.GetCKM(1,0)+VKM.GetCKM(1,1)+VKM.GetCKM(1,2)) );

    double g_db = pdf -> Apdf(0,x1,amu)*(
                  pdf -> Apdf(-1,x2,amu)*(VKM.GetCKM(0,0)+VKM.GetCKM(1,0))
                + pdf -> Apdf(-3,x2,amu)*(VKM.GetCKM(0,1)+VKM.GetCKM(1,1))
                + pdf -> Apdf(-5,x2,amu)*(VKM.GetCKM(0,2)+VKM.GetCKM(1,2)) );

    double db_g = pdf -> Apdf(0,x2,amu)*(
                  pdf -> Apdf(-1,x1,amu)*(VKM.GetCKM(0,0)+VKM.GetCKM(1,0))
                + pdf -> Apdf(-3,x1,amu)*(VKM.GetCKM(0,1)+VKM.GetCKM(1,1))
                + pdf -> Apdf(-5,x1,amu)*(VKM.GetCKM(0,2)+VKM.GetCKM(1,2)) );
    
    
    result[0]=hbarc2*((u_db+db_u)*M2qq(pLabIn, pLabOut, scale)+(g_u+u_g+g_db+db_g))*M2qg(pLabIn, pLabOut, scale);

    return result;
    
}
