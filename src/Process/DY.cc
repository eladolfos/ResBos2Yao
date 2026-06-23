#include "../Process.hh"
#include "../CxFGrid.hh"
#include <mutex>

// Global couplings needed for the matrix element
double DY::CAA[2], DY::CZZ[2], DY::CZA[2];

// Set low range values for VEGAS integration
std::vector<double> DY::GetRangeLow(Settings *s) {
    std::vector<double> range(nDim,0);
    return range;
}

// Set high range values for VEGAS integration
std::vector<double> DY::GetRangeHigh(Settings *s) {
    std::vector<double> range(nDim,1);
    return range;
}

// Function to test the hard part
double DY::METest() {
    return H(91.18);
}

// Calculation of the Hard Part
double DY::H(const double mu) {
    int nf = pdf -> NFL(mu);
    if(HOrder==0) return 1;
    else if(HOrder==1) return 1+pdf->Alpi(mu)*H1();
    else if(HOrder==2) return 1+pdf->Alpi(mu)*(H1()+pdf->Alpi(mu)*H2(nf));
    else throw std::runtime_error("Process: Invalid HOrder value");
}

double DY::H1() {
    return CF*(M_PI*M_PI/2-4);
}

double DY::H2(const int nf) {
    return CF*CA*(59*ZETA3/18.0-1535/192.0+215*M_PI*M_PI/216-pow(M_PI,4)/240)+0.25*CF*CF*(-15*ZETA3+511/16.0-67*M_PI*M_PI/12+17*pow(M_PI,4)/45.0)
        +CF*nf/864.0*(192*ZETA3+1143-152-152*M_PI*M_PI);
}

// Matrix element for Drell-Yan
double DY::ME(double *args, double *wgts) {
    // This section is for testing purposes only
    mB = 91.18; gamB = 2.5;
    double mW2 = pow(80.34,2);
    double gmu = 1.16637E-5;
    double cw2 = mW2/(mB*mB);
    double sw2 = 1.0-cw2;
    double acoupl = sqrt(2)*mW2*2.0*gmu*sw2;
    double zcoupl = M_PI*1.0/(128.0)/sw2/cw2;
    double facA = sqrt(2.0*acoupl)/2.0;
    double qLA=(-1.0/3.0)*facA;
    double qRA=(-1.0/3.0)*facA;
    double eLA=-1*facA;
    double eRA=-1*facA;
    double facZ = sqrt(zcoupl);
    double sw2_eff = 0.231676;
    double sw2_eff_d = sw2_eff-0.0002;
    double eLZ = (-0.5+sw2_eff)*facZ;
    double eRZ = sw2_eff*facZ;
    double qLZ = (-0.5+sw2_eff_d/3.0)*facZ;
    double qRZ = (sw2_eff_d/3.0)*facZ;

    CAA[0] = (qLA*qLA+qRA*qRA)*(eLA*eLA+eRA*eRA);
    CAA[1] = (qLA*qLA-qRA*qRA)*(eLA*eLA-eRA*eRA);
    CZA[0] = (qLZ*qLA+qRZ*qRA)*(eLZ*eLA+eRZ*eRA);
    CZA[1] = (qLZ*qLA-qRZ*qRA)*(eLZ*eLA-eRZ*eRA);
    CZZ[0] = (qLZ*qLZ+qRZ*qRZ)*(eLZ*eLZ+eRZ*eRZ);
    CZZ[1] = (qLZ*qLZ-qRZ*qRZ)*(eLZ*eLZ-eRZ*eRZ);

    //Match variables to that in the original ResBos code for convience of checking
    double Q = args[0], theSta = args[1], phiSta = args[2];
    double SigS = wgts[0], SigA = wgts[1], SigY[5];
    for(int i = 0; i < 5; i++){
        SigY[i] = wgts[i+2];
    }

    double Prop;
    if(mB == 0) Prop = pow(Q,4);
    else Prop = pow(Q*Q-mB*mB,2)+pow(Q,4)*pow(gamB/mB,2);
    double ZZProp = pow(Q,4)/Prop;
    double AAProp = 1.0;
    double ZAProp = pow(Q,2)*(Q*Q-mB*mB)/Prop;

    double alphaEMQ2 = pyalem(Q*Q);
    ZAProp *=(alphaEMQ2/ewAlpha);
    AAProp *=pow(alphaEMQ2/ewAlpha,2);

    double fEven = (
            (1.0 + pow(cos(theSta),2)) *      (SigS + SigY[0]) + // L0
            (1.0 - 3.0*pow(cos(theSta),2))/2.0 *      SigY[2]  + // A0
            sin(2.0*theSta)*cos(phiSta) *             SigY[1]  + // A1
            pow(sin(theSta),2)*cos(2.0*phiSta)/2.0 *  SigY[2] ); // A2
    double fOdd = (
            2.0*cos(theSta) *                 (SigA + SigY[3]) + // A3
            sin(theSta)*cos(phiSta) *                 SigY[4]);  // A4

    double coeffEven = AAProp*CAA[0] + 2.0*ZAProp*CZA[0] + ZZProp*CZZ[0];
    double coeffOdd = AAProp*CAA[1] + 2.0*ZAProp*CZA[1] + ZZProp*CZZ[1];

    return 2.0*(M_PI/3.0)*(4.0*2.0)*(coeffEven*fEven+coeffOdd*fOdd);
}
       
// Calculate the Asymptotic piece for down-type quarks
std::vector<double> ZD::GetAsym(double x1, double x2, double amu, double Qt){
    std::map<int,double> splitSud1, splitSud2;
    std::map<int,double> apdf1, apdf2;
    double d_db, d_db_2, db_d, db_d_2;
    double g_db, g_d, db_g, d_g;

    for(int i = 0; i < 6; i++) {
        apdf1[i] = pdf -> Apdf(i,x1,amu);
        if(i!=0) apdf1[-i] = pdf -> Apdf(-i,x1,amu);
        apdf2[i] = pdf -> Apdf(i,x2,amu);
        if(i!=0) apdf2[-i] = pdf -> Apdf(-i,x2,amu);

        splitSud1[i] = SplitSud(i,x1,amu,Qt);
        if(i!=0) splitSud1[-i] = SplitSud(-i,x1,amu,Qt);
        splitSud2[i] = SplitSud(i,x2,amu,Qt);
        if(i!=0) splitSud2[-i] = SplitSud(-i,x2,amu,Qt);
    }

    if(beam == -1) {
        d_db = splitSud1[1]*apdf2[1]+splitSud1[3]*apdf2[3]+splitSud1[5]*apdf2[5];
        d_db_2 = splitSud2[1]*apdf1[1]+splitSud2[-3]*apdf1[3]+splitSud2[5]*apdf1[5];
        db_d = splitSud1[-1]*apdf2[-1]+splitSud1[-3]*apdf2[-3]+splitSud1[-5]*apdf2[-5];
        db_d_2 = splitSud2[-1]*apdf1[-1]+splitSud2[-3]*apdf1[-3]+splitSud2[-5]*apdf1[-5];

        g_db = splitSud1[0]*(apdf2[1]+apdf2[3]+apdf2[5]);
        g_d = splitSud1[0]*(apdf2[-1]+apdf2[-3]+apdf2[-5]);
        db_g = splitSud2[0]*(apdf1[-1]+apdf1[-3]+apdf1[-5]);
        d_g = splitSud2[0]*(apdf1[1]+apdf1[3]+apdf1[5]);
    } else if(beam == 1) {
        d_db = splitSud1[1]*apdf2[-1]+splitSud1[3]*apdf2[-3]+splitSud1[5]*apdf2[-5];
        d_db_2 = splitSud2[-1]*apdf1[1]+splitSud2[-3]*apdf1[3]+splitSud2[-5]*apdf1[5];
        db_d = splitSud1[-1]*apdf2[1]+splitSud1[-3]*apdf2[3]+splitSud1[-5]*apdf2[5];
        db_d_2 = splitSud2[1]*apdf1[-1]+splitSud2[3]*apdf1[-3]+splitSud2[5]*apdf1[-5];

        g_db = splitSud1[0]*(apdf2[-1]+apdf2[-3]+apdf2[-5]);
        g_d = splitSud1[0]*(apdf2[1]+apdf2[3]+apdf2[5]);
        db_g = splitSud2[0]*(apdf1[-1]+apdf1[-3]+apdf1[-5]);
        d_g = splitSud2[0]*(apdf1[1]+apdf1[3]+apdf1[5]);
        if(fractN != 0) {
            d_db = (1-fractN)*d_db+fractN*(splitSud1[1]*apdf2[-2]+splitSud1[3]*apdf2[-3]+splitSud1[5]*apdf2[-5]);
            d_db_2 = (1-fractN)*d_db_2+fractN*(splitSud2[-2]*apdf1[1]+splitSud2[-3]*apdf1[3]+splitSud2[-5]*apdf1[5]);
            db_d = (1-fractN)*db_d+fractN*(splitSud1[-1]*apdf2[2]+splitSud1[-3]*apdf2[3]+splitSud1[-5]*apdf2[5]);
            db_d_2 = (1-fractN)*db_d_2+fractN*(splitSud2[2]*apdf1[-1]+splitSud2[3]*apdf1[-3]+splitSud2[5]*apdf1[-5]);

            g_db = (1-fractN)*g_db + fractN*(splitSud1[0]*(apdf2[-2]+apdf2[-3]+apdf2[-5]));
            g_d = (1-fractN)*g_d + fractN*(splitSud1[0]*(apdf2[2]+apdf2[3]+apdf2[5]));
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    double asym_1 = d_db+d_db_2+db_d+db_d_2;
    double asym_2 = d_db+d_db_2-db_d-db_d_2;

    double asym_3 = g_db + g_d + db_g + d_g;
    double asym_4 = g_db - g_d - db_g + d_g;

    std::vector<double> asym(2);
    asym[0] = asym_1+asym_3;
    asym[1] = asym_2+asym_4;

    return asym;
}

// Calculate the convolution with collinear functions for down-type quarks
double ZD::CxfCxf(double b, double x1, double x2, int mode) {
    std::map<int,double> convPart1, convPart2;
    double bs=bStar(b);
    double mu=C3/bs;
    int nfEff = pdf -> NFL(mu);
    for(int i = 1; i < 6; i++) {
        if(!fitting) {
            try{
                convPart1[i] = cxfGrid[i]->GetPoint(x1,b,4,4);//SearchCxFMap(i,x1,mu,nfEff);
                convPart1[-i] = cxfGrid[-i]->GetPoint(x1,b,4,4);//SearchCxFMap(-i,x1,mu,nfEff);
                convPart2[i] = cxfGrid[i]->GetPoint(x2,b,4,4);//SearchCxFMap(i,x2,mu,nfEff);
                convPart2[-i] = cxfGrid[-i]->GetPoint(x2,b,4,4);//SearchCxFMap(-i,x2,mu,nfEff);
            } catch(std::runtime_error &error) {
                convPart1[i] = SearchCxFMap(i,x1,mu,nfEff);
                convPart1[-i] = SearchCxFMap(-i,x1,mu,nfEff);
                convPart2[i] = SearchCxFMap(i,x2,mu,nfEff);
                convPart2[-i] = SearchCxFMap(-i,x2,mu,nfEff);
            }
        } else {
            convPart1[i] = SearchCxFMap(i,x1,mu,nfEff);
            convPart1[-i] = SearchCxFMap(-i,x1,mu,nfEff);
            convPart2[i] = SearchCxFMap(i,x2,mu,nfEff);
            convPart2[-i] = SearchCxFMap(-i,x2,mu,nfEff);
        }
    }

    double d_db=0, db_d=0;
    if(beam == -1) {
        d_db = convPart1[1]*convPart2[1]+convPart1[3]*convPart2[3]+convPart1[5]*convPart2[5];
        db_d = convPart2[-1]*convPart1[-1]+convPart2[-3]*convPart1[-3]+convPart2[-5]*convPart1[-5];
    } else if(beam == 1) {
        d_db = convPart1[1]*convPart2[-1]+convPart1[3]*convPart2[-3]+convPart1[5]*convPart2[-5];
        db_d = convPart2[1]*convPart1[-1]+convPart2[3]*convPart1[-3]+convPart2[5]*convPart1[-5];
        if(fractN != 0) {
            d_db = (1-fractN)*d_db+fractN*(convPart1[1]*convPart2[-2]+convPart1[3]*convPart2[-3]+convPart1[5]*convPart2[-5]);
            db_d = (1-fractN)*db_d+fractN*(convPart2[2]*convPart1[-1]+convPart2[3]*convPart1[-3]+convPart2[5]*convPart1[-5]);
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    if(mode == 0) return d_db+db_d;
    else return d_db-db_d;
}

// Calculate the perturbative functions for down-type quarks
std::vector<double> ZD::PLumi1(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    std::vector<double> pLumi(5);

    double d_db=0;
    double db_d=0; 

    if(beam == -1) {
        d_db = pdf->Apdf(1,x1,amu)*pdf->Apdf(1,x2,amu)+pdf->Apdf(3,x1,amu)*pdf->Apdf(3,x2,amu)+pdf->Apdf(5,x1,amu)*pdf->Apdf(5,x2,amu);
        db_d = pdf->Apdf(-1,x1,amu)*pdf->Apdf(-1,x2,amu)+pdf->Apdf(-3,x1,amu)*pdf->Apdf(-3,x2,amu)+pdf->Apdf(-5,x1,amu)*pdf->Apdf(-5,x2,amu);
    } else if(beam == 1) {
        d_db = pdf->Apdf(1,x1,amu)*pdf->Apdf(-1,x2,amu)+pdf->Apdf(3,x1,amu)*pdf->Apdf(-3,x2,amu)+pdf->Apdf(5,x1,amu)*pdf->Apdf(-5,x2,amu);
        db_d = pdf->Apdf(-1,x1,amu)*pdf->Apdf(1,x2,amu)+pdf->Apdf(-3,x1,amu)*pdf->Apdf(3,x2,amu)+pdf->Apdf(-5,x1,amu)*pdf->Apdf(5,x2,amu);
        if(fractN != 0) {
            d_db = (1-fractN)*d_db+fractN*(pdf->Apdf(1,x1,amu)*pdf->Apdf(-2,x2,amu)+pdf->Apdf(3,x1,amu)*pdf->Apdf(-3,x2,amu)+pdf->Apdf(5,x1,amu)*pdf->Apdf(-5,x2,amu));
            db_d = (1-fractN)*db_d+fractN*(pdf->Apdf(-1,x1,amu)*pdf->Apdf(2,x2,amu)+pdf->Apdf(-3,x1,amu)*pdf->Apdf(3,x2,amu)+pdf->Apdf(-5,x1,amu)*pdf->Apdf(5,x2,amu));
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    std::vector<double> amp1QQ = Amp2QQ(sh,th,uh,Q,Qt), amp2QQ = Amp2QQ(sh,uh,th,Q,Qt);
    pLumi[0] = 1.0/sh*(d_db*amp1QQ[0]+db_d*amp2QQ[0]);
    pLumi[1] = 1.0/sh*(d_db*amp1QQ[1]-db_d*amp2QQ[1]);
    pLumi[2] = 1.0/sh*(d_db*amp1QQ[2]+db_d*amp2QQ[2]);
    pLumi[3] = 1.0/sh*(d_db*amp1QQ[3]-db_d*amp2QQ[3]);
    pLumi[4] = 1.0/sh*(d_db*amp1QQ[4]+db_d*amp2QQ[4]);


    return pLumi;
}

// Calculate the perturbative functions for down-type quarks
std::vector<double> ZD::PLumi2(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    std::vector<double> pLumi(5);

    double g_qb=0, g_q=0, q_g=0, qb_g=0;

    if(beam == -1) {
        g_qb = pdf->Apdf(0,x1,amu)*(pdf->Apdf(1,x2,amu)+pdf->Apdf(3,x2,amu)+pdf->Apdf(5,x2,amu));
        g_q  = pdf->Apdf(0,x1,amu)*(pdf->Apdf(-1,x2,amu)+pdf->Apdf(-3,x2,amu)+pdf->Apdf(-5,x2,amu));
        q_g  = pdf->Apdf(0,x2,amu)*(pdf->Apdf(1,x1,amu)+pdf->Apdf(3,x1,amu)+pdf->Apdf(5,x1,amu));
        qb_g = pdf->Apdf(0,x2,amu)*(pdf->Apdf(-1,x1,amu)+pdf->Apdf(-3,x1,amu)+pdf->Apdf(-5,x1,amu));
    } else if(beam == 1) {
        g_qb = pdf->Apdf(0,x1,amu)*(pdf->Apdf(-1,x2,amu)+pdf->Apdf(-3,x2,amu)+pdf->Apdf(-5,x2,amu));
        g_q  = pdf->Apdf(0,x1,amu)*(pdf->Apdf(1,x2,amu)+pdf->Apdf(3,x2,amu)+pdf->Apdf(5,x2,amu));
        q_g  = pdf->Apdf(0,x2,amu)*(pdf->Apdf(1,x1,amu)+pdf->Apdf(3,x1,amu)+pdf->Apdf(5,x1,amu));
        qb_g = pdf->Apdf(0,x2,amu)*(pdf->Apdf(-1,x1,amu)+pdf->Apdf(-3,x1,amu)+pdf->Apdf(-5,x1,amu));
        if(fractN != 0) {
            g_qb = (1-fractN)*g_qb+fractN*(pdf->Apdf(0,x1,amu)*(pdf->Apdf(-2,x2,amu)+pdf->Apdf(-3,x2,amu)+pdf->Apdf(-5,x2,amu)));
            g_q  = (1-fractN)*g_q+fractN*(pdf->Apdf(0,x1,amu)*(pdf->Apdf(2,x2,amu)+pdf->Apdf(3,x2,amu)+pdf->Apdf(5,x2,amu)));
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }
    
    std::vector<double> ampGQ = Amp2GQ(sh,th,uh,Q,Qt), ampQG = Amp2GQ(sh,uh,th,Q,Qt);
    pLumi[0] = 1.0/sh*((g_q+g_qb)*ampGQ[0]+(q_g+qb_g)*ampQG[0]);
    pLumi[1] = 1.0/sh*((g_q+g_qb)*ampGQ[1]-(q_g+qb_g)*ampQG[1]);
    pLumi[2] = 1.0/sh*((g_q+g_qb)*ampGQ[2]+(q_g+qb_g)*ampQG[2]);
    pLumi[3] = 1.0/sh*((-g_q+g_qb)*ampGQ[3]+ (q_g-qb_g)*ampQG[3]);
    pLumi[4] = 1.0/sh*((-g_q+g_qb)*ampGQ[4]+(-q_g+qb_g)*ampQG[4]);

    return pLumi;


}

// Calculate the Asymptotic piece for up-type quarks
std::vector<double> ZU::GetAsym(double x1, double x2, double amu, double Qt){
    std::map<int,double> splitSud1, splitSud2;
    std::map<int,double> apdf1, apdf2;
    double u_ub, u_ub_2, ub_u, ub_u_2;
    double g_ub, g_u, ub_g, u_g;

    for(int i = 0; i < 6; i++) {
        apdf1[i] = pdf -> Apdf(i,x1,amu);
        if(i!=0) apdf1[-i] = pdf -> Apdf(-i,x1,amu);
        apdf2[i] = pdf -> Apdf(i,x2,amu);
        if(i!=0) apdf2[-i] = pdf -> Apdf(-i,x2,amu);

        splitSud1[i] = SplitSud(i,x1,amu,Qt);
        if(i!=0) splitSud1[-i] = SplitSud(-i,x1,amu,Qt);
        splitSud2[i] = SplitSud(i,x2,amu,Qt);
        if(i!=0) splitSud2[-i] = SplitSud(-i,x2,amu,Qt);
    }

    if(beam == -1) {
        u_ub = splitSud1[2]*apdf2[2]+splitSud1[4]*apdf2[4];
        u_ub_2 = splitSud2[2]*apdf1[2]+splitSud2[4]*apdf1[4];
        ub_u = splitSud1[-2]*apdf2[-2]+splitSud1[-4]*apdf2[-4];
        ub_u_2 = splitSud2[-2]*apdf1[-2]+splitSud2[-4]*apdf1[-4];

        g_ub = splitSud1[0]*(apdf2[2]+apdf2[4]);
        g_u = splitSud1[0]*(apdf2[-2]+apdf2[-4]);
        ub_g = splitSud2[0]*(apdf1[-2]+apdf1[-4]);
        u_g = splitSud2[0]*(apdf1[2]+apdf1[4]);
    } else if(beam == 1) {
        u_ub = splitSud1[2]*apdf2[-2]+splitSud1[4]*apdf2[-4];
        u_ub_2 = splitSud2[-2]*apdf1[2]+splitSud2[-4]*apdf1[4];
        ub_u = splitSud1[-2]*apdf2[2]+splitSud1[-4]*apdf2[4];
        ub_u_2 = splitSud2[2]*apdf1[-2]+splitSud2[4]*apdf1[-4];

        g_ub = splitSud1[0]*(apdf2[-2]+apdf2[-4]);
        g_u = splitSud1[0]*(apdf2[2]+apdf2[4]);
        ub_g = splitSud2[0]*(apdf1[-2]+apdf1[-4]);
        u_g = splitSud2[0]*(apdf1[2]+apdf1[4]);
        if(fractN != 0) {
            u_ub = (1-fractN)*u_ub+fractN*(splitSud1[2]*apdf2[-1]+splitSud1[4]*apdf2[-4]);
            u_ub_2 = (1-fractN)*u_ub_2+fractN*(splitSud2[-1]*apdf1[2]+splitSud2[-4]*apdf1[4]);
            ub_u = (1-fractN)*ub_u+fractN*(splitSud1[-2]*apdf2[1]+splitSud1[-4]*apdf2[4]);
            ub_u_2 = (1-fractN)*ub_u_2+fractN*(splitSud2[1]*apdf1[-2]+splitSud2[4]*apdf1[-4]);

            g_ub = (1-fractN)*g_ub + fractN*splitSud1[0]*(apdf2[-1]+apdf2[-4]);
            g_u = (1-fractN)*g_u + fractN*splitSud1[0]*(apdf2[1]+apdf2[4]);
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    double asym_1 = u_ub+u_ub_2+ub_u+ub_u_2;
    double asym_2 = u_ub+u_ub_2-ub_u-ub_u_2;

    double asym_3 = g_ub + g_u + ub_g + u_g;
    double asym_4 = g_ub - g_u - ub_g + u_g;

    std::vector<double> asym(2);
    asym[0] = asym_1+asym_3;
    asym[1] = asym_2+asym_4;

    return asym;
}


std::mutex ZUCxFCxF;
// Calculate the Convolution piece for up-type quarks
double ZU::CxfCxf(double b, double x1, double x2, int mode) {
    double bs=bStar(b);
    double mu=C3/bs;
    int nfEff = pdf -> NFL(mu);

    std::map<int,double> convPart1, convPart2;
    for(int i = 1; i < 6; i++) {
        ZUCxFCxF.lock();
        if(!fitting) {
            try{
                convPart1[i] = cxfGrid[i]->GetPoint(x1,b,4,4);//SearchCxFMap(i,x1,mu,nfEff);
                convPart1[-i] = cxfGrid[-i]->GetPoint(x1,b,4,4);//SearchCxFMap(-i,x1,mu,nfEff);
                convPart2[i] = cxfGrid[i]->GetPoint(x2,b,4,4);//SearchCxFMap(i,x2,mu,nfEff);
                convPart2[-i] = cxfGrid[-i]->GetPoint(x2,b,4,4);//SearchCxFMap(-i,x2,mu,nfEff);
            } catch(std::runtime_error &error) {
                convPart1[i] = SearchCxFMap(i,x1,mu,nfEff);
                convPart1[-i] = SearchCxFMap(-i,x1,mu,nfEff);
                convPart2[i] = SearchCxFMap(i,x2,mu,nfEff);
                convPart2[-i] = SearchCxFMap(-i,x2,mu,nfEff);
            }
        } else {
            convPart1[i] = SearchCxFMap(i,x1,mu,nfEff);
            convPart1[-i] = SearchCxFMap(-i,x1,mu,nfEff);
            convPart2[i] = SearchCxFMap(i,x2,mu,nfEff);
            convPart2[-i] = SearchCxFMap(-i,x2,mu,nfEff);
        }
        ZUCxFCxF.unlock();
    }

    double u_ub=0, ub_u=0;
    if(beam == -1) {
        u_ub = convPart1[2]*convPart2[2]+convPart1[4]*convPart2[4];
        ub_u = convPart2[-2]*convPart1[-2]+convPart2[-4]*convPart1[-4];
    } else if(beam == 1) {
        u_ub = convPart1[2]*convPart2[-2]+convPart1[4]*convPart2[-4];
        ub_u = convPart2[2]*convPart1[-2]+convPart2[4]*convPart1[-4];
        if(fractN != 0) {
            u_ub = (1-fractN)*u_ub+fractN*(convPart1[2]*convPart2[-1]+convPart1[4]*convPart2[-4]);
            ub_u = (1-fractN)*ub_u+fractN*(convPart2[1]*convPart1[-2]+convPart2[4]*convPart1[-4]);
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    if(mode == 0) return u_ub+ub_u;
    else return u_ub-ub_u;
}

// Calculate the perturbative piece for up-type quarks
std::vector<double> ZU::PLumi1(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    std::vector<double> pLumi(5);

    double u_ub=0;
    double ub_u=0; 

    if(beam == -1) {
        u_ub = pdf->Apdf(2,x1,amu)*pdf->Apdf(2,x2,amu)+pdf->Apdf(4,x1,amu)*pdf->Apdf(4,x2,amu);
        ub_u = pdf->Apdf(-2,x1,amu)*pdf->Apdf(-2,x2,amu)+pdf->Apdf(-4,x1,amu)*pdf->Apdf(-4,x2,amu);
    } else if(beam == 1) {
        u_ub = pdf->Apdf(2,x1,amu)*pdf->Apdf(-2,x2,amu)+pdf->Apdf(4,x1,amu)*pdf->Apdf(-4,x2,amu);
        ub_u = pdf->Apdf(-2,x1,amu)*pdf->Apdf(2,x2,amu)+pdf->Apdf(-4,x1,amu)*pdf->Apdf(4,x2,amu);
        if(fractN != 0) {
            u_ub = (1-fractN)*u_ub+fractN*(pdf->Apdf(2,x1,amu)*pdf->Apdf(-1,x2,amu)+pdf->Apdf(4,x1,amu)*pdf->Apdf(-4,x2,amu));
            ub_u = (1-fractN)*ub_u+fractN*(pdf->Apdf(-2,x1,amu)*pdf->Apdf(1,x2,amu)+pdf->Apdf(-4,x1,amu)*pdf->Apdf(4,x2,amu));
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    std::vector<double> amp1QQ = Amp2QQ(sh,th,uh,Q,Qt), amp2QQ = Amp2QQ(sh,uh,th,Q,Qt);
    pLumi[0] = 1.0/sh*(u_ub*amp1QQ[0]+ub_u*amp2QQ[0]);
    pLumi[1] = 1.0/sh*(u_ub*amp1QQ[1]-ub_u*amp2QQ[1]);
    pLumi[2] = 1.0/sh*(u_ub*amp1QQ[2]+ub_u*amp2QQ[2]);
    pLumi[3] = 1.0/sh*(u_ub*amp1QQ[3]-ub_u*amp2QQ[3]);
    pLumi[4] = 1.0/sh*(u_ub*amp1QQ[4]+ub_u*amp2QQ[4]);


    return pLumi;
}

// Calculate the perturbative piece for up-type quarks
std::vector<double> ZU::PLumi2(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    std::vector<double> pLumi(5);

    double g_qb=0, g_q=0, q_g=0, qb_g=0;

    if(beam == -1) {
        g_qb = pdf->Apdf(0,x1,amu)*(pdf->Apdf(2,x2,amu)+pdf->Apdf(4,x2,amu));
        g_q  = pdf->Apdf(0,x1,amu)*(pdf->Apdf(-2,x2,amu)+pdf->Apdf(-4,x2,amu));
        q_g  = pdf->Apdf(0,x2,amu)*(pdf->Apdf(2,x1,amu)+pdf->Apdf(4,x1,amu));
        qb_g = pdf->Apdf(0,x2,amu)*(pdf->Apdf(-2,x1,amu)+pdf->Apdf(-4,x1,amu));
    } else if(beam == 1) {
        g_qb = pdf->Apdf(0,x1,amu)*(pdf->Apdf(-2,x2,amu)+pdf->Apdf(-4,x2,amu));
        g_q  = pdf->Apdf(0,x1,amu)*(pdf->Apdf(2,x2,amu)+pdf->Apdf(4,x2,amu));
        q_g  = pdf->Apdf(0,x2,amu)*(pdf->Apdf(2,x1,amu)+pdf->Apdf(4,x1,amu));
        qb_g = pdf->Apdf(0,x2,amu)*(pdf->Apdf(-2,x1,amu)+pdf->Apdf(-4,x1,amu));
        if(fractN != 0) {
            g_qb = (1-fractN)*g_qb+fractN*(pdf->Apdf(0,x1,amu)*(pdf->Apdf(-1,x2,amu)+pdf->Apdf(-4,x2,amu)));
            g_q  = (1-fractN)*g_q+fractN*(pdf->Apdf(0,x1,amu)*(pdf->Apdf(1,x2,amu)+pdf->Apdf(4,x2,amu)));
        }
    } else {
        throw std::runtime_error("Process: Invalid beam option");
    }

    std::vector<double> ampGQ = Amp2GQ(sh,th,uh,Q,Qt), ampQG = Amp2GQ(sh,uh,th,Q,Qt);
    pLumi[0] = 1.0/sh*((g_q+g_qb)*ampGQ[0]+(q_g+qb_g)*ampQG[0]);
    pLumi[1] = 1.0/sh*((g_q+g_qb)*ampGQ[1]-(q_g+qb_g)*ampQG[1]);
    pLumi[2] = 1.0/sh*((g_q+g_qb)*ampGQ[2]+(q_g+qb_g)*ampQG[2]);
    pLumi[3] = 1.0/sh*((-g_q+g_qb)*ampGQ[3]+ (q_g-qb_g)*ampQG[3]);
    pLumi[4] = 1.0/sh*((-g_q+g_qb)*ampGQ[4]+(-q_g+qb_g)*ampQG[4]);

    return pLumi;


}

// Calculate the asymptotic piece for Z0
std::vector<double> Z0::GetAsym(double x1, double x2, double amu, double Qt){
    SetFlavorCoupling();
    std::vector<double> asymZU = ZU::GetAsym(x1,x2,amu,Qt);
    std::vector<double> asymZD = ZD::GetAsym(x1,x2,amu,Qt);
    std::vector<double> asym(2);
    for(int i = 0; i < 2; i++){
        asym[i] = zeff[0]*asymZU[i]+zeff[1]*asymZD[i];
    }

    return asym;
}

// Calculate the convolution piece for Z0
double Z0::CxfCxf(double b, double x1, double x2, int mode) {
    SetFlavorCoupling();
    return zeff[0]*ZU::CxfCxf(b,x1,x2,mode)+zeff[1]*ZD::CxfCxf(b,x1,x2,mode);
}

// Set couplings for Z0
void Z0::SetFlavorCoupling() {
    sw2Eff = 0.23143;
    double zeffA[2], zeffV[2];
    zeffA[0] = 1.0/2.0;
    zeffV[0] = 1.0/2.0-4.0/3.0*sw2Eff;
    zeffA[1] = -1.0/2.0;
    zeffV[1] = -1.0/2.0+2.0/3.0*sw2Eff;

    for(int i = 0; i < 2; i++) {
        zeff[i] = pow(zeffA[i],2)+pow(zeffV[i],2);
    }
}

// Calculate the perturbative piece for Z0
std::vector<double> Z0::PLumi1(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    SetFlavorCoupling();
    std::vector<double> pLumi(5);
    std::vector<double> pLumiZU = ZU::PLumi1(x1,x2,sh,th,uh,amu,Q,Qt);
    std::vector<double> pLumiZD = ZD::PLumi1(x1,x2,sh,th,uh,amu,Q,Qt);

    for(int i = 0; i < 5; i++) pLumi[i] = zeff[0]*pLumiZU[i]+zeff[1]*pLumiZD[i];

    return pLumi;
}

// Calculate the perturbative piece for Z0
std::vector<double> Z0::PLumi2(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    SetFlavorCoupling();
    std::vector<double> pLumi(5);
    std::vector<double> pLumiZU = ZU::PLumi2(x1,x2,sh,th,uh,amu,Q,Qt);
    std::vector<double> pLumiZD = ZD::PLumi2(x1,x2,sh,th,uh,amu,Q,Qt);

    for(int i = 0; i < 5; i++) pLumi[i] = zeff[0]*pLumiZU[i]+zeff[1]*pLumiZD[i];

    return pLumi;
}

// Define couplings for Z0
void Z0::SetCoupling(double Q) {
    ewDelRho = 3.0*gmu*mt*mt/8.0/sqrt(2)/M_PI/M_PI;
    ewDelRho *= (1.0-2.0/3.0*(M_PI*M_PI/3.0+1.0)*pdf->Alpi(mt));
    coupl = M_PI/3.0*sqrt(2)*mB*mB*gmu*(1.0+ewDelRho);

}

// Calculate asymptotic piece for photon
std::vector<double> A0::GetAsym(double x1, double x2, double amu, double Qt){
    aeff[0] = pow(2.0/3.0,2); aeff[1] = pow(1.0/3.0,2);
    std::vector<double> asymZU = ZU::GetAsym(x1,x2,amu,Qt);
    std::vector<double> asymZD = ZD::GetAsym(x1,x2,amu,Qt);
    std::vector<double> asym(2);
    for(int i = 0; i < 2; i++){
        asym[i] = aeff[0]*asymZU[i]+aeff[1]*asymZD[i];
    }

    return asym;
}

// Set photon coupling
void A0::SetCoupling(double Q) {
    coupl = M_PI*M_PI*pyalem(Q)*4.0/3.0*pyalem(Q)/3.0/M_PI/Q/Q;
}

// Calculate convolution piece for photon
double A0::CxfCxf(double b, double x1, double x2, int mode) {
    aeff[0] = pow(2.0/3.0,2); aeff[1] = pow(1.0/3.0,2);
    return aeff[0]*ZU::CxfCxf(b,x1,x2,mode)+aeff[1]*ZD::CxfCxf(b,x1,x2,mode);
}

// Calculate perturbative piece for photon
std::vector<double> A0::PLumi1(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    aeff[0] = pow(2.0/3.0,2); aeff[1] = pow(1.0/3.0,2);
    std::vector<double> pLumi(5);
    std::vector<double> pLumiZU = ZU::PLumi1(x1,x2,sh,th,uh,amu,Q,Qt);
    std::vector<double> pLumiZD = ZD::PLumi1(x1,x2,sh,th,uh,amu,Q,Qt);

    for(int i = 0; i < 5; i++) pLumi[i] = aeff[0]*pLumiZU[i]+aeff[1]*pLumiZD[i];

    return pLumi;
}

// Calculate perturbative piece for photon
std::vector<double> A0::PLumi2(double x1, double x2, double sh, double th, double uh, double amu, double Q, double Qt) {
    aeff[0] = pow(2.0/3.0,2); aeff[1] = pow(1.0/3.0,2);
    std::vector<double> pLumi(5);
    std::vector<double> pLumiZU = ZU::PLumi2(x1,x2,sh,th,uh,amu,Q,Qt);
    std::vector<double> pLumiZD = ZD::PLumi2(x1,x2,sh,th,uh,amu,Q,Qt);

    for(int i = 0; i < 5; i++) pLumi[i] = aeff[0]*pLumiZU[i]+aeff[1]*pLumiZD[i];

    return pLumi;
}
