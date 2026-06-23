#include <cmath>

#include "ResBos/Beam.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/PhaseSpace.hh"
#include "ResBos/Process/Wpm.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {

REGISTER(WPlus)
REGISTER(WMinus)

// General functions for both W+ and W-
Wpm::Wpm(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) : Process(resbos_, settings) {}

// Process specific initialization
void Wpm::Initialize(const IO::Settings &settings) {
    // Construct phase space object for the given process
    ps = std::unique_ptr<PhaseSpace>(new PhaseSpace(pow(resbos -> GetECM(),2)));
    std::vector<double> Mass = {GetEW() -> GetMwReal(),0,0};
    ps->SetMass(Mass);
    ps->SetQRange(settings.GetSettingDouble("Qmin"), settings.GetSettingDouble("Qmax"));
    ps->SetQtRange(settings.GetSettingDouble("Qtmin"), settings.GetSettingDouble("Qtmax"));

    // Define couplings using EW class
    fL2 = std::abs(GetEW() -> GetWCoupl());
    coupling = M_PI*fL2*2.0/3.0;

    DLOG_F(INFO,"W Coupling: %f",sqrt(fL2));
}

double Wpm::ME(const std::vector<double> &psPoint, const std::vector<double> &wgt) const {
    // psPoint[0] = Q
    // psPoint[1] = q_T
    // psPoint[2] = y
    // psPoint[3] = theta_star
    // psPoint[4] = phi_star (Not used in resummed or asymptotic piece)
    double Q_V = psPoint[0];
    double mw = GetEW() -> GetMwReal();
    double gamW = GetEW() -> GetGamW();
    double Propagator = pow(Q_V*Q_V-mw*mw,2)+pow(Q_V,4)*pow(gamW/mw,2);

    double SigS = wgt[0];
    double SigA = wgt[1];
    double the_sta = psPoint[3];

    return 4.0*pow(Q_V,4)/Propagator 
        * ( fL2 * (1.0 + pow(cos(the_sta),2)) * SigS    // L0
          + sign * fL2 * (2.0*cos(the_sta)) * SigA);     // A3
}

double Wpm::H1() const { 
    return QCD::CF*(QCD::pi2/2.0-4); 
}

double Wpm::H2(const double &Q) const {
    const size_t nf = resbos -> GetNF(Q);
    return H2(nf);
}

double Wpm::H2(const size_t &nf) const {
    return QCD::CF*QCD::CA*(59*QCD::ZETA3/18.0-1535/192.0+215*QCD::pi2/216.0-pow(QCD::pi2,2)/240.0)
        + 0.25*QCD::CF*QCD::CF*(-15*QCD::ZETA3+511.0/16.0-67*QCD::pi2/12.0+17*pow(QCD::pi2,2)/45.0)
        + QCD::CF*static_cast<double>(nf)/864.0*(192*QCD::ZETA3+1143-152*QCD::pi2);
}

double Wpm::CxFCxF(const double &mu, const double &x1, const double &x2, const size_t &mode) const{
    static double total1, total2;
    if(mode % 2 == 0) {
        total1 = 0; total2 = 0;
        Partons partons = GetPartons(mode);
        for(auto parton : partons) {
            double ckm = GetEW() -> GetCKM(parton.first, parton.second);
            total1 += ckm * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.first,x1,mu)
                   * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.second,x2,mu);
            total2 += ckm * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.first,x2,mu)
                   * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.second,x1,mu);
        }
        return total1+total2;
    }
    return total1 - total2;
}

double Wpm::GetPhaseSpacePoint(const std::vector<double> &x, std::vector<double>& Point, 
                               std::vector<FourVector>& momenta) const {
    return ps -> Wresph(x, GetEW() -> GetGamW(), Point, momenta);
}

double Wpm::GetPert(const double &x1, const double &x2, const double &sh, const double &th,
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
        double ckm = GetEW() -> GetCKM(parton.first, parton.second);
        qqb +=  1.0/sh*( ckm *
                resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.first,x1,mu)*
                resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.second,x2,mu)*
                Amp2QQ(sh,th,uh,Q,qt,mode) + signQQB*ckm*
                resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.second,x1,mu)*
                resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.first,x2,mu)*
                Amp2QQ(sh,uh,th,Q,qt,mode));

        gqb += ckm*gPDF1*resbos->GetBeams().second->PDF(Beam::BDF::PDF,parton.second,x2,mu);
        qbg += ckm*gPDF2*resbos->GetBeams().first->PDF(Beam::BDF::PDF,parton.second,x1,mu);

        gq  += ckm*gPDF1*resbos->GetBeams().second->PDF(Beam::BDF::PDF,parton.first,x2,mu);
        qg  += ckm*gPDF2*resbos->GetBeams().first->PDF(Beam::BDF::PDF,parton.first,x1,mu);
    }

    double gContrib = 1.0/sh * (
            (signG1*gq + signG3*gqb) * Amp2GQ(sh,th,uh,Q,qt,mode) +
            (signG2*qg + signG4*qbg) * Amp2GQ(sh,uh,th,Q,qt,mode));

    return qqb + gContrib;
}

double Wpm::Amp2QQ(const double &sh, const double &th, const double &uh, const double &Q, 
                   const double &qt, const size_t &mode) const {
    double result = (pow(sh+uh,2) + pow(sh+th,2))/uh/th; // = t/u + u/t + 2sQ^2/(ut)
    double mt = sqrt(Q*Q + qt*qt);

    double factor = 1.0;
    if(mode % nAngular == 1) 
        factor = qt*Q/mt/mt*(pow(Q*Q-uh,2)-pow(Q*Q-th,2))/(pow(Q*Q-uh,2)+pow(Q*Q-th,2));
    else if(mode % nAngular == 2)
        factor = qt*qt/mt/mt;
    else if(mode % nAngular == 3)
        factor = Q/mt;
    else if(mode % nAngular == 4)
        factor = 2*qt/mt*(pow(Q*Q-uh,2)-pow(Q*Q-th,2))/(pow(Q*Q-uh,2)+pow(Q*Q-th,2));

    return factor*result*2.0/3.0/M_PI;
}

double Wpm::Amp2GQ(const double &sh, const double &th, const double &uh, const double &Q,
                   const double &qt, const size_t &mode) const {
    double result = -(pow(sh+th,2)+pow(uh+th,2))/uh/sh/4.0/M_PI;
    double mt = sqrt(Q*Q + qt*qt);

    double factor = 1.0;
    if(mode % nAngular == 1) 
        factor = qt*Q/mt/mt*(2.0*pow(sh+th,2)-pow(sh+uh,2))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2));
    else if(mode % nAngular == 2)
        factor = qt*qt/mt/mt*(pow(Q*Q+sh,2)+pow(Q*Q-uh,2))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2));
    else if(mode % nAngular == 3)
        factor = Q/mt*(1.0 - 2.0*uh*(Q*Q-sh)/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2)));
    else if(mode % nAngular == 4)
        factor = 2*qt/mt*(1.0+(2.0*sh*(Q*Q-sh))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2)));

    return factor*result;
}

Partons WPlus::GetPartons(const size_t&) const {
    static Partons partons;
    if(partons.empty()) {
        partons.push_back(std::make_pair(2,-1));
        partons.push_back(std::make_pair(2,-3));
        partons.push_back(std::make_pair(2,-5));
        partons.push_back(std::make_pair(4,-1));
        partons.push_back(std::make_pair(4,-3));
        partons.push_back(std::make_pair(4,-5));
    }
    return partons;
}

Partons WMinus::GetPartons(const size_t&) const {
    static Partons partons;
    if(partons.empty()) {
        partons.push_back(std::make_pair(1,-2));
        partons.push_back(std::make_pair(3,-2));
        partons.push_back(std::make_pair(5,-2));
        partons.push_back(std::make_pair(1,-4));
        partons.push_back(std::make_pair(3,-4));
        partons.push_back(std::make_pair(5,-4));
    }
    return partons;
}

void Wpm::MakeEvent(Event &event, const std::vector<FourVector>& momenta) {
    Particle WBoson(pids[0],momenta[0],2), 
             lepton(pids[1],momenta[1],1),
             neutrino(pids[2],momenta[2],1);

    WBoson.SetDaughters(2,3);
    lepton.SetMothers(1,0);
    neutrino.SetMothers(1,0);

    event.AddParticle(WBoson);
    event.AddParticle(lepton);
    event.AddParticle(neutrino);
}


}
