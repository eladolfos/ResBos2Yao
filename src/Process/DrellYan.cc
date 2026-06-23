#include <cmath>

#include "ResBos/Beam.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/PhaseSpace.hh"
#include "ResBos/Process/DrellYan.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {

REGISTER(DrellYan)

DrellYan::DrellYan(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) : Process(resbos_, settings) {
    name = "DrellYan";
}

// Process specific initialization
void DrellYan::Initialize(const IO::Settings &settings) {
    // Construct phase space object for the given process
    ps = std::unique_ptr<PhaseSpace>(new PhaseSpace(pow(resbos -> GetECM(),2)));
    std::vector<double> Mass = {GetEW() -> GetMzReal(),0,0};
    ps->SetMass(Mass);
    ps->SetQRange(settings.GetSettingDouble("Qmin"), settings.GetSettingDouble("Qmax"));
    ps->SetQtRange(settings.GetSettingDouble("Qtmin"), settings.GetSettingDouble("Qtmax"));
    ps->SetPhiDep(true);
    ps->SetNSecondaryPart(0);

    // Define couplings using EW class
    double sws = abs(GetEW() -> GetSW2());
    double FAC = sqrt(abs(GetEW() -> GetZCoupl()));
    double FAC_A = sqrt(2.0*abs(GetEW() -> GetACoupl()))/2.0;

    double EL_Z = (-0.5 + sws)*FAC;
    double ER_Z = sws*FAC;

    QL_Z1 = (0.5 - 2.0/3.0*sws)*FAC; 
    QR_Z1 = (-2.0/3.0*sws)*FAC;
    QL_Z2 = (-0.5 + 1.0/3.0*sws)*FAC;
    QR_Z2 = (sws/3.0)*FAC;

    double QL_A1 = (2.0/3.0)*FAC_A;
    double QR_A1 = (2.0/3.0)*FAC_A;
    double QL_A2 = (-1.0/3.0)*FAC_A;
    double QR_A2 = (-1.0/3.0)*FAC_A;
    double EL_A = -1.0*FAC_A;
    double ER_A = -1.0*FAC_A;

    double xle = 2*EL_Z/FAC;
    double xre = 2*ER_Z/FAC;
    double temp = pow(GetEW() -> GetMzReal(), 3)/sqrt(2)/12/M_PI*GetEW() -> GetGF();
    branching_ratio = temp*(xle*xle+xre*xre)/GetEW() -> GetGamZ();

    // 0 corresponds to up-type quarks, 1 corresponds to down-type quarks
    CAA_E[0] = ( QL_A1*QL_A1 + QR_A1*QR_A1 )*( EL_A*EL_A + ER_A*ER_A ); 
    CAA_O[0] = ( QL_A1*QL_A1 - QR_A1*QR_A1 )*( EL_A*EL_A - ER_A*ER_A );
    CZZ_E[0] = ( QL_Z1*QL_Z1 + QR_Z1*QR_Z1 )*( EL_Z*EL_Z + ER_Z*ER_Z );
    CZZ_O[0] = ( QL_Z1*QL_Z1 - QR_Z1*QR_Z1 )*( EL_Z*EL_Z - ER_Z*ER_Z );
    CZA_E[0] = ( QL_Z1*QL_A1 + QR_Z1*QR_A1 )*( EL_Z*EL_A + ER_Z*ER_A );
    CZA_O[0] = ( QL_Z1*QL_A1 - QR_Z1*QR_A1 )*( EL_Z*EL_A - ER_Z*ER_A );
    CAA_E[1] = ( QL_A2*QL_A2 + QR_A2*QR_A2 )*( EL_A*EL_A + ER_A*ER_A );
    CAA_O[1] = ( QL_A2*QL_A2 - QR_A2*QR_A2 )*( EL_A*EL_A - ER_A*ER_A );
    CZZ_E[1] = ( QL_Z2*QL_Z2 + QR_Z2*QR_Z2 )*( EL_Z*EL_Z + ER_Z*ER_Z );
    CZZ_O[1] = ( QL_Z2*QL_Z2 - QR_Z2*QR_Z2 )*( EL_Z*EL_Z - ER_Z*ER_Z );
    CZA_E[1] = ( QL_Z2*QL_A2 + QR_Z2*QR_A2 )*( EL_Z*EL_A + ER_Z*ER_A );
    CZA_O[1] = ( QL_Z2*QL_A2 - QR_Z2*QR_A2 )*( EL_Z*EL_A - ER_Z*ER_A );

    LOG_F(INFO,"EL = %e, ER = %e", EL_Z, ER_Z);
    LOG_F(INFO,"Photon Coupling to up quarks and electrons: %e, %e",CAA_E[0],CAA_O[0]);
    LOG_F(INFO,"Photon Coupling to down quarks and electrons: %e, %e",CAA_E[1],CAA_O[1]);
    LOG_F(INFO,"Z Coupling to up quarks and electrons: %e, %e",CZZ_E[0],CZZ_O[0]);
    LOG_F(INFO,"Z Coupling to down quarks and electrons: %e, %e",CZZ_E[1],CZZ_O[1]);
    LOG_F(INFO,"Z-A Interference Coupling to up quarks and electrons: %e, %e",CZA_E[0],CZA_O[0]);
    LOG_F(INFO,"Z-A Interference Coupling to down quarks and electrons: %e, %e",CZA_E[1],CZA_O[1]);
    LOG_F(INFO, "BR_Z_EE = %e", branching_ratio);
}

// Process specific initialization for MCFM Interface
void DrellYan::InitializeMCFM() {
    // Define couplings using EW class
    double sws = abs(GetEW() -> GetSW2());
    double FAC = sqrt(abs(GetEW() -> GetZCoupl()));
    double FAC_A = sqrt(2.0*abs(GetEW() -> GetACoupl()))/2.0;

    double EL_Z = (-0.5 + sws)*FAC;
    double ER_Z = sws*FAC;

    QL_Z1 = (0.5 - 2.0/3.0*sws)*FAC; 
    QR_Z1 = (-2.0/3.0*sws)*FAC;
    QL_Z2 = (-0.5 + 1.0/3.0*sws)*FAC;
    QR_Z2 = (sws/3.0)*FAC;

    double QL_A1 = (2.0/3.0)*FAC_A;
    double QR_A1 = (2.0/3.0)*FAC_A;
    double QL_A2 = (-1.0/3.0)*FAC_A;
    double QR_A2 = (-1.0/3.0)*FAC_A;
    double EL_A = -1.0*FAC_A;
    double ER_A = -1.0*FAC_A;

    double xle = -1+2*abs(GetEW() -> GetSW2());
    double xre = 2*abs(GetEW() -> GetSW2());
    double temp = pow(GetEW() -> GetMzReal(), 3)/sqrt(2)/12/M_PI*GetEW() -> GetGF();
    branching_ratio = temp*(xle*xle+xre*xre)/GetEW() -> GetGamZ();
    // 0 corresponds to up-type quarks, 1 corresponds to down-type quarks
    CAA_E[0] = ( QL_A1*QL_A1 + QR_A1*QR_A1 )*( EL_A*EL_A + ER_A*ER_A ); 
    CAA_O[0] = ( QL_A1*QL_A1 - QR_A1*QR_A1 )*( EL_A*EL_A - ER_A*ER_A );
    CZZ_E[0] = ( QL_Z1*QL_Z1 + QR_Z1*QR_Z1 )*( EL_Z*EL_Z + ER_Z*ER_Z );
    CZZ_O[0] = ( QL_Z1*QL_Z1 - QR_Z1*QR_Z1 )*( EL_Z*EL_Z - ER_Z*ER_Z );
    CZA_E[0] = ( QL_Z1*QL_A1 + QR_Z1*QR_A1 )*( EL_Z*EL_A + ER_Z*ER_A );
    CZA_O[0] = ( QL_Z1*QL_A1 - QR_Z1*QR_A1 )*( EL_Z*EL_A - ER_Z*ER_A );
    CAA_E[1] = ( QL_A2*QL_A2 + QR_A2*QR_A2 )*( EL_A*EL_A + ER_A*ER_A );
    CAA_O[1] = ( QL_A2*QL_A2 - QR_A2*QR_A2 )*( EL_A*EL_A - ER_A*ER_A );
    CZZ_E[1] = ( QL_Z2*QL_Z2 + QR_Z2*QR_Z2 )*( EL_Z*EL_Z + ER_Z*ER_Z );
    CZZ_O[1] = ( QL_Z2*QL_Z2 - QR_Z2*QR_Z2 )*( EL_Z*EL_Z - ER_Z*ER_Z );
    CZA_E[1] = ( QL_Z2*QL_A2 + QR_Z2*QR_A2 )*( EL_Z*EL_A + ER_Z*ER_A );
    CZA_O[1] = ( QL_Z2*QL_A2 - QR_Z2*QR_A2 )*( EL_Z*EL_A - ER_Z*ER_A );
}

double DrellYan::NonPert(const std::array<double, 2> &correction, const std::vector<double> &calc, bool pert) const {
    return 4*M_PI/3.0*((QL_Z1*QL_Z1+QR_Z1*QR_Z1)*correction[0]*calc[0]
                      +(QL_Z2*QL_Z2+QR_Z2*QR_Z2)*correction[1]*calc[pert ? 5 : 2]);
}

std::array<double, 4> DrellYan::CalcCorrection(double Q, const std::vector<double> &calc, bool pert) const {
    size_t idx2 = pert ? 5 : 2;
    const double alpha0 = GetEW() -> GetAlphaEM(0);
    const double mz = GetEW() -> GetMzReal();
    const double gamZ = GetEW() -> GetGamZ();
    const double Propagator = pow(Q*Q-mz*mz,2)+pow(Q,4)*pow(gamZ/mz,2);

    const double zzProp = pow(Q,4)/Propagator;
    double aaProp = 1.0;
    double zaProp = Q*Q*(Q*Q-mz*mz)/Propagator;

    const double alphaQ2 = GetEW() -> GetAlphaEM(Q*Q);
    zaProp *= alphaQ2/alpha0;
    aaProp *= pow(alphaQ2/alpha0,2);

    const double coeff0 = aaProp*CAA_E[0] + 2.0*zaProp*CZA_E[0] + zzProp*CZZ_E[0];
    const double coeff1 = aaProp*CAA_E[1] + 2.0*zaProp*CZA_E[1] + zzProp*CZZ_E[1];

    return {coeff0*calc[0], coeff1*calc[idx2], zzProp*CZZ_E[0]*calc[0], zzProp*CZZ_E[1]*calc[idx2]};
}

double DrellYan::ME(const std::vector<double> &psPoint, const std::vector<double> &wgt) const {
    // psPoint[0] = Q
    // psPoint[1] = q_T
    // psPoint[2] = y
    // psPoint[3] = theta_star
    // psPoint[4] = phi_star (Not used in resummed or asymptotic piece)
    const double alpha0 = GetEW() -> GetAlphaEM(0);
    double Q = psPoint[0];
    double mz = GetEW() -> GetMzReal();
    double gamZ = GetEW() -> GetGamZ();
    double Propagator = pow(Q*Q-mz*mz,2)+pow(Q,4)*pow(gamZ/mz,2);

    double zzProp = pow(Q,4)/Propagator;
    double aaProp = 1.0;
    double zaProp = Q*Q*(Q*Q-mz*mz)/Propagator;

    double alphaQ2 = GetEW() -> GetAlphaEM(Q*Q);
    zaProp *= alphaQ2/alpha0;
    aaProp *= pow(alphaQ2/alpha0,2);

    // Write the SigS and SigA so that it would work for DY, ZU, and ZD
    std::vector<double> SigL0(nChannels,0), SigA0(nChannels,0), SigA1(nChannels,0);
    std::vector<double> SigA2(nChannels,0), SigA3(nChannels,0), SigA4(nChannels,0);
    for(size_t i = 0; i < nChannels; ++i) {
        for(size_t j = 0; j < wgt.size()/nChannels; ++j) {
            switch(j) {
                case 0:
                    SigL0[i] = wgt[i*wgt.size()/nChannels+j];
                    break;
                case 1:
                    SigA4[i] = wgt[i*wgt.size()/nChannels+j];
                    break;
                case 3:
                    SigA1[i] = wgt[i*wgt.size()/nChannels+j];
                    break;
                case 2:
                    SigA0[i] = wgt[i*wgt.size()/nChannels+j];
                    SigA2[i] = wgt[i*wgt.size()/nChannels+j];
                    break;
                case 4:
                    SigA3[i] = wgt[i*wgt.size()/nChannels+j];
                    break;
            }
        }
    }
    double the_sta = psPoint[3];
    double phi_sta = psPoint[4];

    // Write calculation of matrix element so it works for DY, ZU, and ZD
    double angularOdd, angularEven, coeffEven, coeffOdd;
    double total = 0;
    for(size_t i = 0; i < nChannels; ++i) {
        size_t iMode = i;
        if(name == "ZD") iMode += 1;

        angularEven = (1.0+pow(cos(the_sta),2))*SigL0[i]                 // L0
                    + (1.0-3.0*pow(cos(the_sta),2))/2.0*SigA0[i]         // A0
                    + sin(2*the_sta)*cos(phi_sta)*SigA1[i]               // A1
                    + pow(sin(the_sta),2)*cos(2.0*phi_sta)/2.0*SigA2[i]; // A2

        angularOdd = sin(the_sta)*cos(phi_sta)*SigA3[i]                  // A3
                   + 2.0*cos(the_sta)*SigA4[i];                          // A4

        coeffEven = aaProp*CAA_E[iMode] + 2.0*zaProp*CZA_E[iMode] + zzProp*CZZ_E[iMode];
        coeffOdd = aaProp*CAA_O[iMode] + 2.0*zaProp*CZA_O[iMode] + zzProp*CZZ_O[iMode];
        total += coeffEven*angularEven + coeffOdd*angularOdd;
    }

    double FACTOR=2.0*(M_PI/3.0)*(4.0*2.0);

    return FACTOR*total;
}

Partons DrellYan::GetPartons(const size_t &i) const {
    if(i==0) {
        static Partons partonsUp;
        if(partonsUp.empty()) {
            partonsUp.push_back(std::make_pair(2,-2));
            partonsUp.push_back(std::make_pair(4,-4));
        }
        return partonsUp;
    } else {
        static Partons partonsDw;
        if(partonsDw.empty()) {
            partonsDw.push_back(std::make_pair(1,-1));
            partonsDw.push_back(std::make_pair(3,-3));
            partonsDw.push_back(std::make_pair(5,-5));
        }
        return partonsDw;
    }
}

double DrellYan::CxFCxF(const double &mu, const double &x1, const double &x2,
                        const size_t &mode) const {
    static double total1, total2;
    if(mode % 2 == 0) {
        total1 = 0; total2 = 0;
        Partons partons = GetPartons(mode/2);
        for(auto parton : partons) {
            total1 += resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.first,x1,mu)
                   * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.second,x2,mu);
            total2 += resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.first,x2,mu)
                   * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.second,x1,mu);
        }
        return total1 + total2;
    }
    return total1 - total2;
}

double DrellYan::GetPhaseSpacePoint(const std::vector<double> &x, std::vector<double>& Point,
                                    std::vector<FourVector>& momenta) const {
    return ps -> Wresph(x, GetEW() -> GetGamZ(), Point, momenta);
}

double DrellYan::H1() const { 
    return QCD::CF*(QCD::pi2/2.0-4); 
}

double DrellYan::H2(const double &Q) const {
    const size_t nf = resbos -> GetNF(Q);
    return H2(nf);
}

double DrellYan::H2(const size_t &nf) const {
    return QCD::CF*QCD::CA*(59*QCD::ZETA3/18.0-1535/192.0+215*QCD::pi2/216.0-pow(QCD::pi2,2)/240.0)
        + 0.25*QCD::CF*QCD::CF*(-15*QCD::ZETA3+511.0/16.0-67*QCD::pi2/12.0+17*pow(QCD::pi2,2)/45.0)
        + QCD::CF*static_cast<double>(nf)/864.0*(192*QCD::ZETA3+1143-152*QCD::pi2);
}
        
double DrellYan::GetPert(const double &x1, const double &x2, const double &sh, const double &th,
                         const double &uh, const double &mu, const double &Q, const double &qt,
                         const size_t &mode) const {
    double qqb = 0, qg = 0, gq = 0, qbg = 0, gqb = 0;
    double gPDF1 = resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,0,x1,mu);
    double gPDF2 = resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,0,x2,mu);
    int signQQB = 1, signG1 = 1, signG2 = 1, signG3 = 1, signG4 = 1;
    // Order so that the order of the vector matches that of the original ResBos grid
    if(mode % nAngular == 3) {
        signQQB = -1;
        signG2 = -1;
        signG4 = -1;
    } else if(mode % nAngular == 1) {
        signQQB = -1;
        signG1 = -1;
        signG4 = -1;
    } else if(mode % nAngular == 4) {
        signG1 = -1;
        signG2 = -1;
    }
    Partons partons = GetPartons(mode/nAngular);
    for(auto parton : partons) {
        qqb +=  1.0/sh*(
                resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.first,x1,mu)*
                resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.second,x2,mu)*
                Amp2QQ(sh,th,uh,Q,qt,mode) + signQQB*
                resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.second,x1,mu)*
                resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.first,x2,mu)*
                Amp2QQ(sh,uh,th,Q,qt,mode));

        gqb += gPDF1*resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.second,x2,mu);
        gq  += gPDF1*resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.first,x2,mu);

        qg  += gPDF2*resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.first,x1,mu);
        qbg += gPDF2*resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.second,x1,mu);
    }

    double gContrib = 1.0/sh * (
            (signG1*gq + signG3*gqb) * Amp2GQ(sh,th,uh,Q,qt,mode) +
            (signG2*qg + signG4*qbg) * Amp2GQ(sh,uh,th,Q,qt,mode));

    return qqb + gContrib;
}

double DrellYan::Amp2QQ(const double &sh, const double &th, const double &uh, const double &Q,
                        const double &qt, const size_t &mode) const {
    double result = (pow(sh+uh,2) + pow(sh+th,2))/uh/th; // = t/u + u/t + 2sQ^2/(ut)
    double mt = sqrt(Q*Q + qt*qt);

    double factor = 1.0;
    // Order so that the order of the vector matches that of the original ResBos grid
    if(mode % nAngular == 3) 
        factor = qt*Q/mt/mt*(pow(Q*Q-uh,2)-pow(Q*Q-th,2))/(pow(Q*Q-uh,2)+pow(Q*Q-th,2));
    else if(mode % nAngular == 2)
        factor = qt*qt/mt/mt;
    else if(mode % nAngular == 1)
        factor = Q/mt;
    else if(mode % nAngular == 4)
        factor = 2*qt/mt*(pow(Q*Q-uh,2)-pow(Q*Q-th,2))/(pow(Q*Q-uh,2)+pow(Q*Q-th,2));

    return factor*result*2.0/3.0/M_PI;
}

double DrellYan::Amp2GQ(const double &sh, const double &th, const double &uh, const double &Q,
                        const double &qt, const size_t &mode) const {
    double result = -(pow(sh+th,2)+pow(uh+th,2))/uh/sh/4.0/M_PI;
    double mt = sqrt(Q*Q + qt*qt);

    double factor = 1.0;
    // Order so that the order of the vector matches that of the original ResBos grid
    if(mode % nAngular == 3) 
        factor = qt*Q/mt/mt*(2.0*pow(sh+th,2)-pow(sh+uh,2))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2));
    else if(mode % nAngular == 2)
        factor = qt*qt/mt/mt*(pow(Q*Q+sh,2)+pow(Q*Q-uh,2))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2));
    else if(mode % nAngular == 1)
        factor = Q/mt*(1.0 - 2.0*uh*(Q*Q-sh)/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2)));
    else if(mode % nAngular == 4)
        factor = 2.0*qt/mt*(1.0+(2.0*sh*(Q*Q-sh))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2)));

    return factor*result;
}

void DrellYan::MakeEvent(Event &event, const std::vector<FourVector>& momenta) {
    Particle ZBoson(pids[0],momenta[0],2), 
             lepton(pids[1],momenta[1],1),
             alepton(pids[2],momenta[2],1);

    ZBoson.SetDaughters(2,3);
    lepton.SetMothers(1,0);
    alepton.SetMothers(1,0);

    event.AddParticle(ZBoson);
    event.AddParticle(lepton);
    event.AddParticle(alepton);
}

REGISTER(ZU)

Partons ZU::GetPartons(const size_t&) const {
    static Partons partons;
    if(partons.empty()) {
        partons.push_back(std::make_pair(2,-2));
        partons.push_back(std::make_pair(4,-4));
    }
    return partons;
}

double ZU::CxFCxF(const double &mu, const double &x1, const double &x2, const size_t &mode) const {
    static double total1, total2;
    if(mode % 2 == 0) {
        total1 = 0; total2 = 0;
        Partons partons = GetPartons(mode/2);
        for(auto parton : partons) {
            total1 += resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.first,x1,mu)
                   * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.second,x2,mu);
            total2 += resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.first,x2,mu)
                   * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.second,x1,mu);
        }
        return total1 + total2;
    }
    return total1 - total2;
}

REGISTER(ZD)

Partons ZD::GetPartons(const size_t&) const {
    static Partons partons;
    if(partons.empty()) {
        partons.push_back(std::make_pair(1,-1));
        partons.push_back(std::make_pair(3,-3));
        partons.push_back(std::make_pair(5,-5));
    }
    return partons;
}

double ZD::CxFCxF(const double &mu, const double &x1, const double &x2, const size_t &mode) const {
    static double total1, total2;
    if(mode % 2 == 0) {
        total1 = 0; total2 = 0;
        Partons partons = GetPartons(mode/2);
        for(auto parton : partons) {
            total1 += resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.first,x1,mu)
                   * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.second,x2,mu);
            total2 += resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.first,x2,mu)
                   * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.second,x1,mu);
        }
        return total1 + total2;
    }
    return total1 - total2;
}

}
