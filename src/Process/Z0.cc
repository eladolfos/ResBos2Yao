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

REGISTER(Z0)

Z0::Z0(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) : Process(resbos_,settings) {
    name = "Z0";
}

// Process specific initialization
void Z0::Initialize([[maybe_unused]]const IO::Settings &settings) {
    ps = std::unique_ptr<PhaseSpace>(new PhaseSpace(pow(resbos -> GetECM(),2)));
    std::vector<double> Mass = {GetEW() -> GetMzReal(),0,0};
    ps->SetMass(Mass);
    ps->SetQRange(settings.GetSettingDouble("Qmin"), settings.GetSettingDouble("Qmax"));
    ps->SetQtRange(settings.GetSettingDouble("Qtmin"), settings.GetSettingDouble("Qtmax"));

    // Define couplings using EW class
    double sw2 = std::abs(GetEW() -> GetSW2());
    double delta_rho = 3*GetEW() -> GetGF()*pow(GetEW() -> GetMtReal(), 2)/8/sqrt(2)/pow(M_PI, 2);
    delta_rho *= 1 - 2/3*(pow(M_PI, 2)/3+1)*resbos -> GetAlpi(GetEW() -> GetMtReal());
    double delta_ka = std::abs(GetEW() -> GetCW2()/GetEW() -> GetSW2())*delta_rho;
    double sw2_eff = sw2*(1.0+delta_ka);
    LOG_F(INFO, "Sin^2(w) = %e", sw2);
    LOG_F(INFO, "delta rho = %e", delta_rho);
    LOG_F(INFO, "delta ka = %e", delta_ka);
    LOG_F(INFO, "Sin^2(w)_eff = %e", sw2_eff);

    ZFFA[1] = -0.5;
    ZFFA[2] = 0.5;
    ZFFA[3] = -0.5;
    ZFFA[4] = 0.5;
    ZFFA[5] = -0.5;

    ZFFV[1] = -0.5 + 2.0/3.0 * sw2_eff;
    ZFFV[2] = 0.5 - 4.0/3.0 * sw2_eff;
    ZFFV[3] = -0.5 + 2.0/3.0 * sw2_eff;
    ZFFV[4] = 0.5 - 4.0/3.0 * sw2_eff;
    ZFFV[5] = -0.5 + 2.0/3.0 * sw2_eff;
}

double Z0::Charge(const int &pid, const size_t &mode) const {
    if(mode % 2 == 0) {
        return pow(ZFFV.at(pid),2) + pow(ZFFA.at(pid),2);
    } else {
        return 2.0*ZFFV.at(pid)*ZFFA.at(pid);
    }
}

double Z0::ME(const std::vector<double> &psPoint, const std::vector<double> &wgt) const {
    // psPoint[0] = Q
    // psPoint[1] = q_T
    // psPoint[2] = y
    // psPoint[3] = theta_star
    // psPoint[4] = phi_star (Not used in resummed or asymptotic piece)
    // TODO: Implement this
    return 1;
}

double Z0::GetCoupling(const double&) const {
    return M_PI*std::abs(GetEW() -> GetZCoupl())/3.0;
}

Partons Z0::GetPartons(const size_t&) const {
    static Partons partons;
    if(partons.empty()) {
        partons.push_back(std::make_pair(2,-2));
        partons.push_back(std::make_pair(4,-4));
        partons.push_back(std::make_pair(1,-1));
        partons.push_back(std::make_pair(3,-3));
        partons.push_back(std::make_pair(5,-5));
    }
    return partons;
}

double Z0::CxFCxF(const double &mu, const double &x1, const double &x2, const size_t &mode) const {
    static double total1, total2;
    if(mode % 2 == 0) {
        total1 = 0; total2 = 0;
        Partons partons = GetPartons(mode/2);
        for(auto parton : partons) {
            total1 += Charge(parton.first, mode)
                   * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.first,x1,mu)
                   * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.second,x2,mu);
            total2 += Charge(parton.first, mode)
                   * resbos -> GetBeams().second -> PDF(Beam::BDF::C,parton.first,x2,mu)
                   * resbos -> GetBeams().first -> PDF(Beam::BDF::C,parton.second,x1,mu);
        }
        return total1 + total2;
    }
    return total1 - total2;
}

double Z0::GetPhaseSpacePoint(const std::vector<double> &x, std::vector<double>& Point,
                              std::vector<FourVector>& momenta) const {
    return ps -> Wresph(x, GetEW() -> GetGamZ(), Point, momenta);
}

double Z0::H1() const { 
    return QCD::CF*(QCD::pi2/2.0-4); 
}

double Z0::H2(const double &Q) const {
    const size_t nf = resbos -> GetNF(Q);
    return H2(nf);
}

double Z0::H2(const size_t &nf) const {
    return QCD::CF*QCD::CA*(59.0*QCD::ZETA3/18.0-1535.0/192.0+215.0*QCD::pi2/216.0
            -pow(QCD::pi2,2)/240.0)
        + 0.25*QCD::CF*QCD::CF*(-15*QCD::ZETA3+511.0/16.0-67*QCD::pi2/12.0
                +17*pow(QCD::pi2,2)/45.0)
        + QCD::CF*static_cast<double>(nf)/864.0*(192*QCD::ZETA3+1143-152*QCD::pi2);
}

double Z0::GetPert(const double &x1, const double &x2, const double &sh, const double &th,
                   const double &uh, const double &mu, const double &Q, const double &qt,
                   const size_t &mode) const {
    double qqb = 0, qg = 0, gq = 0, qbg = 0, gqb = 0;
    double gPDF1 = resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,0,x1,mu);
    double gPDF2 = resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,0,x2,mu);
    int signQQB = 1, signG1 = 1, signG2 = 1, signG3 = 1, signG4 = 1;
    // Order so that the order of the vector matches that of the original ResBos grid
    if(mode % nAngular == 3) {
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
                Charge(parton.first, mode) *
                resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.first,x1,mu)*
                resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.second,x2,mu)*
                Amp2QQ(sh,th,uh,Q,qt,mode) + signQQB*
                Charge(parton.first, mode) *
                resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.second,x1,mu)*
                resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.first,x2,mu)*
                Amp2QQ(sh,uh,th,Q,qt,mode));

        gqb += gPDF1*resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.second,x2,mu)
             * Charge(parton.first,mode);
        gq  += gPDF1*resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,parton.first,x2,mu)
             * Charge(parton.first,mode);

        qg  += gPDF2*resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.first,x1,mu)
             * Charge(parton.first,mode);
        qbg += gPDF2*resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,parton.second,x1,mu)
             * Charge(parton.first,mode);
    }

    double gContrib = 1.0/sh * (
            (signG1*gq + signG3*gqb) * Amp2GQ(sh,th,uh,Q,qt,mode) +
            (signG2*qg + signG4*qbg) * Amp2GQ(sh,uh,th,Q,qt,mode));

    return qqb + gContrib;
}

double Z0::Amp2QQ(const double &sh, const double &th, const double &uh, const double &Q,
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

double Z0::Amp2GQ(const double &sh, const double &th, const double &uh, const double &Q,
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
        factor = 2*qt/mt*(1.0+(2.0*sh*(Q*Q-sh))/(pow(Q*Q-sh,2)+pow(Q*Q-uh,2)));

    return factor*result;
}

void Z0::MakeEvent(Event &event, const std::vector<FourVector>& momenta) {
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

}
