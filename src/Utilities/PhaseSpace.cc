#include <cmath>

#include "ResBos/PhaseSpace.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/ThreeVector.hh"
#include "ResBos/Random.hh"

#include "ResBos/loguru.hpp"

#define pi M_PI

// Default constructor for PhaseSpace class
PhaseSpace::PhaseSpace() {
    rand = new Random();
}

PhaseSpace::PhaseSpace(const double& S_, const size_t& iMonte_,
                       const size_t& iQt_, const size_t& nPrimaryPart_) :
    S{S_}, iMonte{iMonte_}, iQt{iQt_}, nPrimaryPart{nPrimaryPart_} {
    rand = new Random();

    //TODO: Handle these two correctly by calling the process or something like that
    phiDep = false;
    nSecondaryPart = 0;
}

double PhaseSpace::PhaseGen(const std::vector<double> &x, std::vector<FourVector> &p) const {
    // Initialize
    static int init=0;
    static double z[200];
    static double acc=1E-18;
    static int itmax = 6;
    if(init==0) {
        z[0] = 0;
        for(int i = 1; i < 200; i++) {
            z[i] = z[i-1]+log(static_cast<double>(i));
        }
        init=1;
    }

    // Count Nonzero masses
    double xmt=0;
    int nm = 0;
    for(size_t i = 0; i < nPrimaryPart; i++) {
        if(mass[i] > 1E-6) {
            nm++;
            xmt+=mass[i];
        }
    }
    if(xmt>sqrt(S)) return 0;

    // Generate nPrimaryPart-1 Massless Momenta in infinite phase space
    double extraWeight = 1;
    for(size_t i = 0; i < nPrimaryPart-1; i++) {
        double cs, sn, f, fac;
        if(i==0) {
            cs=2.0*x[2]-1;
            sn=sqrt(1-cs*cs);
            f=0; // There is one redundant overall phi
            fac=x[3];
        } else {
            cs=2.0*x[1+3*i]-1;
            sn=sqrt(1-cs*cs);
            f=2.0*pi*x[2+3*i];
            fac=x[3+3*i];
        }
        double Q=-log(fac);
        extraWeight*=Q;
        ThreeVector q;
        q.SetPxPyPz(Q*sn*sin(f),Q*sn*cos(f),Q*cs);
        p[i].SetVectM(q,0);
    }

    //Calculate the parameters of the scale transformation
    double r[4] = {0};
    for(size_t i=0; i < nPrimaryPart-1; i++) {
        for(size_t k=0; k<4; k++) {
            r[k]+=p[i][k];
        }
    }

    //Calculate the nth vector
    for(size_t k=0; k<3; k++) {
        p[nPrimaryPart-1][k]=-r[k];
    }
    p[nPrimaryPart-1][3]=sqrt(pow(p[nPrimaryPart-1][0],2)+pow(p[nPrimaryPart-1][1],2)+pow(p[nPrimaryPart-1][2],2));
    r[3]+=p[nPrimaryPart-1][3];
    double rmas=r[3];
    if(rmas==0) return 0;
    double y = sqrt(S)/rmas;
    
    //Scale the Q's to the P's
    for(size_t i = 0; i < nPrimaryPart; i++) {
        for(size_t j = 0; j < 4; j++) {
            p[i][j]*=y;
        }
    }

    //Return for weighted massless momenta
    double wt=log(2*pi)*static_cast<double>(nPrimaryPart-1)
             -log(sqrt(S)-p[nPrimaryPart-1][3])*2*static_cast<double>(1-nPrimaryPart)
             -log(sqrt(S))-log(2)-log(p[nPrimaryPart-1][3])-z[2*nPrimaryPart-3];
    if(nm==0) return extraWeight*exp(wt);

    //Massive Particles: Rescale the momenta by a factor
    double xmax=sqrt(1-pow(xmt/sqrt(S),2));
    std::vector<double> xm2(nPrimaryPart), p2(nPrimaryPart);
    for(size_t i = 0; i < nPrimaryPart; i++) {
       xm2[i]=pow(mass[i],2);
       p2[i]=pow(p[i][3],2);
    }
    int iter=0;
    y=xmax;
    double accu=sqrt(S)*acc;
    std::vector<double> e(nPrimaryPart);
    while(iter<itmax){
        double f0=-sqrt(S);
        double g0=0;
        double y2=y*y;
        for(size_t i = 0; i < nPrimaryPart; i++) {
            e[i]=sqrt(xm2[i]+y2*p2[i]);
            f0+=e[i];
            if(e[i]>0)g0+=p2[i]/e[i];
        }
        if(fabs(f0)>accu&&iter<itmax){
            iter++;
            y=y-f0/(y*g0);
        }else if(fabs(f0)<=accu) {
            iter=itmax+1;
        }
    }
    std::vector<double> v(nPrimaryPart);
    for(size_t i = 0; i < nPrimaryPart; i++) {
        v[i]=y*p[i][3];
        for(size_t k=0;k<3;k++){
            p[i][k]=y*p[i][k];
        }
        p[i][3]=e[i];
    }

    //Calculate the mass-effect weight factor
    double wt2=1.0, wt3=0;
    for(size_t i=0; i<nPrimaryPart;i++){
        if(e[i]>0){
            wt2*=v[i]/e[i];
            wt3+=v[i]*v[i]/e[i];
        }
    }
    double wtm=static_cast<double>(2*nPrimaryPart-3)*log(y)+log(wt2/wt3*sqrt(S));

    //Return for weighted massive momenta
    wt+=wtm;
    wt=exp(wt)*extraWeight;
    if(xmt>sqrt(S)) return 0;
    else return wt;
}

// Three body phase space generation, preformed by assuming that it is actually two 1->2 phase spaces 
double PhaseSpace::PhaseSpace3(const std::vector<double> &x, double &x1, double &x2,
                               std::vector<FourVector> &p){
    //Mass[0] is mass of intermediate boson 
    //Mass[1] is mass of p3 
    //Mass[2] is mass of p4
    //Mass[3] is mass of p5
    if(qMin < mass[1]+mass[2]+mass[3]) qMin = mass[1]+mass[2]+mass[3];
    double x1x2min=(pow(qMin,2))/S;
    double w = sqrt(S);
    double zTemp=1.0/((1.0/x1x2min-1.0)*x[5]+1.0);
    x1=pow(zTemp,x[6]);
    x2=zTemp/x1;
    double jacobian=-pow(zTemp,2)*log(zTemp)*(1.0/x1x2min-1.0);

    p[0].SetPxPyPzE(0,0,0.5*w*x1,0.5*w*x1);
    p[1].SetPxPyPzE(0,0,-0.5*w*x2,0.5*w*x2);

    std::array<double,5> r;
    for(size_t i=0; i<5; i++){
        r[i]=x[i];
    }

    double wt=FillMomenta3(r, p);

    return wt*jacobian*0.5/(x1*x2*S);
}

// Fill in the momenta for the three-body decay
double PhaseSpace::FillMomenta3(const std::array<double,5> &r, std::vector<FourVector> &p) const{
    FourVector pLab, pCM1,pCM2;
    pLab=p[0]+p[1];
    double sh = pLab[3]*pLab[3]-pLab[2]*pLab[2];
    double rsh = sqrt(sh);

    //Default Shape
    double yM34 =qMin+(qMax-qMin)*r[0];
    double wtYM34=2.0*yM34*(qMax-qMin);

    if(rsh < yM34 || yM34 < (mass[1]+mass[2])){
        return 0;
    }

    //Generate: p1 p2 --> p34 p5
    double rrsh[2];
    for(size_t i = 0; i < 2; i++) {
        rrsh[i]=r[i+1]; 
    }

    FourVector pWp;
    double wt1 = x2Phase(rrsh,rsh,mass[3],yM34,p[4],pWp);

    //Generate: p34 --> p3 p4
    double rt[2];
    for(size_t i=0;i<2;i++){
        rt[i]=r[i+3];
    }


    double wt2 = x2Phase(rt,yM34,mass[1],mass[2],p[2],p[3]);

    //boost from p34 rest frame to CM frame 
    p[2].Boost(pWp.BoostVector()); 
    //top=pWp-bottom;
    p[3].Boost(pWp.BoostVector());


    //Fill in vectors in the lab frame
    pCM1.SetPxPyPzE(0,0,rsh/2.0,rsh/2.0);
    pCM2.SetPxPyPzE(0,0,-rsh/2.0,rsh/2.0);

    //boost outgoing momentum to the lab frame
    p[2].Boost(pLab.BoostVector());
    p[3].Boost(pLab.BoostVector());
    p[4].Boost(pLab.BoostVector());

    return wtYM34*wt1*wt2/(2.0*pi);
}

// Two body phase space generator
double PhaseSpace::x2Phase(double *x, const double &m12, const double &m1,
                           const double &m2, FourVector &p1, FourVector &p2) const {
    double costh=2.0*x[0]-1.0;
    double sinth=sqrt(1-costh*costh);
    double phiSta=2.0*pi*x[1];
    double rsh=m12;
    double sh=rsh*rsh;
    double temp1=(sh+m1*m1-m2*m2)/2.0/rsh;
    double q1cm=sqrt(temp1*temp1-m1*m1);
    double sphi1=sin(phiSta);
    double cphi1=cos(phiSta);
    p1.SetPxPyPzE(q1cm*cphi1*sinth,q1cm*sphi1*sinth,q1cm*costh,temp1);
    p2=-p1;
    p2[3]=m12-p1[3];

    double dPhase2=q1cm/(4.0*rsh*pow(2.0*pi,6));
    double yJaco1=2.0;
    double yJaco2=2.0*pi;
    return yJaco1*yJaco2*pow(2.0*pi,4)*dPhase2;
}

// Specialized phase space generator for Vector boson resummation
double PhaseSpace::Wresph(const std::vector<double> &x, const double &Yg,
                          std::vector<double> &phaseSpace, std::vector<FourVector> &p){
    double YM = mass[0];
//    double YM1 = Mass[1];
//    double YM2 = Mass[2];

    double Q2=0, Q, WtQ2=0;
    double yMinQ, yMaxQ, yVar, yNorm;
    if(qMax == qMin) {
        qMax += Yg;
        qMin -= Yg;
    }
    switch(iMonte){
        case 0:
        case 3:
            Q2 = qMin*qMin+(qMax*qMax-qMin*qMin)*x[0];
            WtQ2 = (qMax*qMax-qMin*qMin);
            break;
        case 1:
            if(qMin == 0) qMin = 1E-7;
            Q2 = qMin*qMin*pow(qMax/qMin,2*x[0]);
            WtQ2 = Q2*log(pow(qMax/qMin,2));
            break;
        case 2:
            yMinQ = (pow(qMin,2)-YM*YM)/(YM*Yg);
            yMaxQ = (pow(qMax,2)-YM*YM)/(YM*Yg);
            yVar = atan(yMaxQ)*x[0]+(1-x[0])*atan(yMinQ);
            yNorm = atan(yMaxQ)-atan(yMinQ);
            Q2 = YM*YM+YM*Yg*tan(yVar);
            WtQ2 = YM*Yg*yNorm/(pow(cos(yVar),2));
            break;
    }
    Q = sqrt(Q2);
    if(Q < mass[1]+mass[2]) return 0;

    double bQT2 = (5.9-1.2)/(qtMax*qtMax-0.1);
    double qtMax2 = qtMax*qtMax;
    double temp0, temp, cQT, qt2=0, qt, WtQt2=0, alpha1=0.2,temp1;
    double qtMin2 = qtMin*qtMin;

    switch(iQt){
        case 0:
            temp0 = exp(-bQT2*qtMin2);
            temp = temp0-exp(-bQT2*qtMax2);
            cQT = bQT2/temp;
            qt2 = -log(temp0-temp*x[1])/bQT2;
            WtQt2 = 1.0/cQT/(temp0-temp*x[1]);
            break;
        case 1:
            temp0 = pow(log(qtMin2/Q2),2);
            temp = pow(log(qtMax2/Q2),2)-temp0;
            cQT = 2.0/temp;
            temp1 = sqrt(temp0+temp*x[1]);
            qt2 = Q2*exp(-temp1);
            WtQt2 = -1.0/cQT/temp1*qt2;
            break;
        case 2:
            qt2 = qtMin2*pow(qtMax2/qtMin2,x[1]);
            WtQt2 = qt2*log(qtMax2/qtMin2);
            break;
        case 3:
            temp0 = pow(qtMin2/qtMax2,alpha1);
            temp1 = temp0 + (1.0-temp0)*x[1];
            temp = pow(temp1,1.0/alpha1);
            qt2 = qtMax2*temp;
            WtQt2 = qtMax2*(1.0-temp0)*temp/alpha1/temp1;
            break;
    }

    qt = sqrt(qt2);

    double yMax = YMaximum(Q,qt);

    double yb = yMax*(2.0*x[2]-1.0);
    double WtYb = 2.0*yMax;
   
    double r[2];
    r[0] = x[3];

    // if(phiDep) r[1] = x[4];
    // else r[1] = rand->Get();
    r[1] = x[4];

    double theSta = acos(2.0*r[0]-1.0);
    double phiSta = 2.0*pi*r[1];
    double sThe = sin(theSta), cThe = cos(theSta);
    double sPhi = sin(phiSta), cPhi = cos(phiSta);

    double wtThe = 2.0, wtPhi = 2.0*pi;

    if(phaseSpace.size() != 5) phaseSpace.resize(5);
    phaseSpace[0] = Q;
    phaseSpace[1] = qt;
    phaseSpace[2] = yb;
    phaseSpace[3] = theSta;
    phaseSpace[4] = phiSta;

    double wt = pi/2.0*WtQt2*WtQ2*WtYb*wtThe*wtPhi;

    double pCMl1 = sqrt((Q2-pow(mass[1]+mass[2],2))*(Q2-pow(mass[1]-mass[2],2)))/2.0/Q;
    double ECMl1 = (Q2+pow(mass[1],2)-pow(mass[2],2))/2.0/Q;
    double ECMl2 = (Q2-pow(mass[1],2)+pow(mass[2],2))/2.0/Q;

    double beta = 2.0*pCMl1/Q;

    wt*=1.0/pow(2.0*pi,3)/8.0*beta/(pi*qt)/(2*Q2);

    double r2inv = 1.0/sqrt(2.0);

    FourVector pNbar(0,0,r2inv,r2inv), pN(0,0,-r2inv,r2inv);

    //Give a random choice of azimuthal angle in lab frame
    double phiQ = 2.0*pi*rand->Get();
    double tm2Q=Q2+qt2;
    double tmQ = sqrt(tm2Q);

    FourVector pL1CS(pCMl1*sThe*cPhi,pCMl1*sThe*sPhi,pCMl1*cThe,ECMl1);
    FourVector pQ(qt*cos(phiQ),qt*sin(phiQ),tmQ*sinh(yb),tmQ*cosh(yb));
    
    // X, Y, Z unit vectors in the CS frame, given in the lab frame
    // X = -Q/(Qt Mt) * (q+ nmu + q- nbarmu - MT^2/Q^2 qmu)
    FourVector pX(tmQ*cos(phiQ)/Q,tmQ*sin(phiQ)/Q,pQ.Pz()*qt/tmQ/Q,pQ.E()*qt/tmQ/Q);
    FourVector pZ(0,0,pQ.E()/tmQ,pQ.Pz()/tmQ);
    FourVector pY = GetYVect(Q,pQ,pX,pZ);

    FourVector pL1, pL2;
    for(size_t i = 0; i < 4; i++) {
        pL1[i] = ECMl1*pQ[i]/Q+pCMl1*(sThe*cPhi*pX[i]+sThe*sPhi*pY[i]+cThe*pZ[i]); 
        pL2[i] = ECMl2*pQ[i]/Q-pCMl1*(sThe*cPhi*pX[i]+sThe*sPhi*pY[i]+cThe*pZ[i]); 
    }

    // std::cout << "pl1: " << pL1 << " pl2: " << pL2 << std::endl;
    // std::cout << "Px: " << pX << " Py: " << pY << " Pz: " << pZ << std::endl;

    //Check the masses
    if(mass[1] > 1E-8 || mass[2] > 1E-8) {
        if(std::abs(pL1.M()-mass[1]) > pL1.M()*1E-8 ||
           std::abs(pL2.M()-mass[2]) > pL2.M()*1E-8) {
            std::cerr << "PhaseSpace: Potential error in mass: pL1 or pL2" << std::endl;
            return 0;
        }
    } else if(pL1.M() > 1E-3 || pL2.M() > 1E-3) {
            std::cerr << "PhaseSpace: Potential error in mass: pL1 or pL2" << std::endl;
            return 0;
    }

    if(p.size() != 3) p.resize(3);
    p[0].SetPxPyPzE(pQ.Px(),pQ.Py(),pQ.Pz(),pQ.E());
    p[1].SetPxPyPzE(pL1.Px(),pL1.Py(),pL1.Pz(),pL1.E());
    p[2].SetPxPyPzE(pL2.Px(),pL2.Py(),pL2.Pz(),pL2.E());

    if(nSecondaryPart != 0) AdditionalDecays();

    return wt;
}

// Function to calculate the LeviCevita symbol, and can actually be done at compile time for speed
constexpr int LeviCevita(const int i, const int j, const int k, const int l) {
    return (i==j||i==k||i==l||j==k||j==l||k==l) ? 0 : -(i-j)*(i-k)*(i-l)*(j-k)*(j-l)*(k-l)/12;
}

// Calculate the remaining vector needed for the transformation between CS-Frame and Lab Frame
FourVector PhaseSpace::GetYVect(const double &dQ, const FourVector &Q,
                                const FourVector &X, const FourVector &Z) const {
    FourVector Y = {0,0,0,0};
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++) {
            for(size_t k = 0; k < 4; k++) {
                for(size_t l = 0; l < 4; l++) {
                    int LC = LeviCevita(static_cast<int>(i), static_cast<int>(j),
                                        static_cast<int>(k), static_cast<int>(l));
                    if(LC==0) continue;
                    Y[l]+=Q[i]*X[j]*Z[k]/dQ*LC;
                }
            }
        }
    }

    Y[3] = -Y[3];

    return Y;

}

// Calculate the maximum allowed rapidity for vector boson production
double PhaseSpace::YMaximum(const double &q, const double &qT) const {
    double t1 = S+q*q;
    double t2 = sqrt(qT*qT+q*q);
    double t3 = 2*sqrt(S)*t2;

    return acosh(t1/t3);
}

