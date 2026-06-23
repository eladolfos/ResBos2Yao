#include <cmath>
#include <complex>

#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/HiggsBranching.hh"
#include "ResBos/PhaseSpace.hh"
#include "ResBos/Process/Higgs.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {

REGISTER(Higgs)

Higgs::Higgs(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) : Process(resbos_, settings) {
    name = "Higgs";
}

// Process specific initialization
void Higgs::Initialize(const IO::Settings &settings) {
    // Load branching ratio results from HDECAY calculation
    std::vector<double> mhIn, BrAA;    
    LOG_F(INFO,"Loading branching ratio for Higgs to diphoton");
    BrHGrid = MakeGrid(); 

    // Construct phase space object for the given process
    ps = std::unique_ptr<PhaseSpace>(new PhaseSpace(pow(resbos -> GetECM(),2)));
    double mass1 = 0, mass2 = 0;
    if(settings.GetSettingString("HiggsDecay") == "WW") {
        mass1 = GetEW() -> GetMwReal();
        mass2 = GetEW() -> GetMwReal();
        nDim = 11;
        higgsDecay = HiggsDecay::WW;
        LOG_F(INFO,"Higgs decay mode set to WW");
    } else if(settings.GetSettingString("HiggsDecay") == "ZZ") {
        mass1 = GetEW() -> GetMzReal();
        mass2 = GetEW() -> GetMzReal();
        nDim = 11;
        higgsDecay = HiggsDecay::ZZ;
        LOG_F(INFO,"Higgs decay mode set to ZZ");
    } else if(settings.GetSettingString("HiggsDecay") == "AA") {
        mass1 = 0;
        mass2 = 0;
        nDim = 5;
        higgsDecay = HiggsDecay::AA;
        pids = {25,22,22};
        LOG_F(INFO,"Higgs decay mode set to AA");
    } else {
        throw std::runtime_error("Higgs: Invalid Decay. Currently, options are WW, ZZ, AA");
    }

    std::vector<double> Mass = {GetEW() -> GetMhReal(), mass1, mass2};
    ps -> SetMass(Mass);
    ps -> SetQRange(settings.GetSettingDouble("Qmin"), settings.GetSettingDouble("Qmax"));
    ps -> SetQtRange(settings.GetSettingDouble("Qtmin"), settings.GetSettingDouble("Qtmax"));
}

// Process specific initialization for MCFM Interface
void Higgs::InitializeMCFM() {

}

double Higgs::ME(const std::vector<double> &psPoint, const std::vector<double> &wgt) const {
    if(higgsDecay == HiggsDecay::AA) {
        // Input array for HiggsDecay::AA
        // psPoint[0] = Q
        // psPoint[1] = q_T
        // psPoint[2] = y
        // psPoint[3] = theta_star
        // psPoint[4] = phi_star (Not used in resummed or asymptotic piece)
        double Q = psPoint[0];
        double mH = GetEW() -> GetMhReal();
        double gH = GetEW() -> GetGamH();
        double propagator = pow(Q*Q-mH*mH,2)+pow(Q,4)*pow(gH/mH,2);

        double wt = MassCorrections(Q)*wgt[0]/propagator*32.0*M_PI*pow(Q,3)*gH;
        return wt*BrH2AA(Q);
    } else if(higgsDecay == HiggsDecay::WW || higgsDecay == HiggsDecay::ZZ) {

    } else throw std::runtime_error("Higgs: Decay mode not implemented");
    
    return 0;
}
        
double Higgs::GetCoupling(const double &Q) const {
    double result = sqrt(2.0)*GetEW() -> GetGF()*M_PI/576.0;
    result *= pow(resbos -> GetAlpi(Q)*Q,2);
    //TODO Add in scale dependence here
    return result;
}

Partons Higgs::GetPartons(const size_t&) const {
    Partons partons;
    partons.push_back(std::make_pair(0,0));
    return partons;
}

double Higgs::CxFCxF(const double &mu, const double &x1, const double &x2, const size_t&) const {
    double result = resbos -> GetBeams().first -> PDF(Beam::BDF::C,0,x1,mu)
                  * resbos -> GetBeams().second -> PDF(Beam::BDF::C,0,x2,mu);
    if(resbos -> GetCalculation() -> GetCOrder() > 1) {
        result += (resbos -> GetBeams().first -> PDF(Beam::BDF::G1,0,x1,mu)
         * resbos -> GetBeams().second -> PDF(Beam::BDF::G1,0,x2,mu))
         * pow(resbos->GetAlpi(mu),2);
    }
    return result;
}

double Higgs::GetPhaseSpacePoint(const std::vector<double> &x, std::vector<double>& Point,
                                 std::vector<FourVector>& momenta) const {
    return ps -> Wresph(x, GetEW() -> GetGamH(), Point, momenta);
}

double Higgs::H1() const {
    // This is the value in the limit mt->infinity
    return QCD::CA*M_PI*M_PI/2 + 11.0/2.0;
}

double Higgs::H2(const double &Q) const {
    // This is the value in the limit mt->infinity
    using namespace QCD;
    const double LQ = 2.0*log(Q/GetEW() -> GetMtReal());
    const double nf = static_cast<double>(resbos -> GetNF(Q));
    return CA*CA*(3187.0/288.0+7.0/8.0*LQ+157.0/72.0*pi2+13.0/144.0*pi2*pi2-55.0/18.0*ZETA3)
         + CA*CF*(-145.0/24.0-11.0/8.0*LQ-3.0/4.0*pi2)
         + 9.0/4.0*CF*CF-5.0/96.0*CA-1.0/12.0*CF
         - CA*nf*(287.0/144.0+5.0/36.0*pi2+4.0/9.0*ZETA3)
         + CF*nf*(-41.0/24.0-0.5*LQ+ZETA3);
}

double Higgs::GetPert(const double &x1, const double &x2, const double &sh, const double &th,
                      const double &uh, const double &mu, const double &Q,
                      const double&, const size_t&) const {
    double qg = 0, gq = 0, qqb = 0, qbq = 0;
    double gPDF1 = resbos -> GetBeams().first -> PDF(Beam::BDF::PDF,0,x1,mu);
    double gPDF2 = resbos -> GetBeams().second -> PDF(Beam::BDF::PDF,0,x2,mu);

    double gg =  1.0/sh*gPDF1*gPDF2*Amp2GG(sh,th,uh,Q);

    for(int pid = 1; pid < 6; ++pid) {
        gq += gPDF1*resbos->GetBeams().second->PDF(Beam::BDF::PDF,pid,x2,mu);
        gq += gPDF1*resbos->GetBeams().second->PDF(Beam::BDF::PDF,-pid,x2,mu);

        qg += gPDF2*resbos->GetBeams().first->PDF(Beam::BDF::PDF,pid,x1,mu);
        qg += gPDF2*resbos->GetBeams().first->PDF(Beam::BDF::PDF,-pid,x1,mu);

        qqb += resbos->GetBeams().first->PDF(Beam::BDF::PDF,pid,x1,mu)
             * resbos->GetBeams().first->PDF(Beam::BDF::PDF,-pid,x2,mu);
        qbq += resbos->GetBeams().first->PDF(Beam::BDF::PDF,-pid,x1,mu)
             * resbos->GetBeams().first->PDF(Beam::BDF::PDF,pid,x2,mu);
    }

    return gg + 1.0/sh*(gq*Amp2GQ(sh,th,uh,Q)+qg*Amp2GQ(sh,uh,th,Q)
            + (qqb+qbq)*Amp2QQ(sh,th,uh,Q));
}

double Higgs::Amp2GG(const double &sh, const double &th, const double &uh, const double &Q) const {
    double result = (pow(Q,8)+pow(sh,4)+pow(th,4)+pow(uh,4))/sh/th/uh;
    return result*3.0/2.0/M_PI/Q/Q;
}

double Higgs::Amp2GQ(const double &sh, const double &th, const double &uh, const double &Q) const {
    double result = -(sh*sh+uh*uh)/th;
    return result*2.0/3.0/M_PI/Q/Q;
}

double Higgs::Amp2QQ(const double &sh, const double &th, const double &uh, const double &Q) const {
    double result = (th*th+uh*uh)/sh;
    return result*16.0/9.0/M_PI/Q/Q;
}

double Higgs::MassCorrections(const double &Q) const {
    const std::complex<double> i(0,1);
    std::complex<double> A(0,0);
    std::vector<double> masses = {resbos -> GetSetting<double>("mc"),
                                  resbos -> GetSetting<double>("mb"),
                                  resbos -> GetSetting<double>("mt")};
   
    for(auto mass : masses) {
        std::complex<double> t = Q*Q/mass/mass/4.0;
        std::complex<double> f = t.real() <= 1.0 ? pow(sin(sqrt(t)),2) : -pow(log((1.0+sqrt(1.0-t))/(1.0-sqrt(1.0-t)))-i*M_PI,2);
        A += 2.0*(t + (t-1.0) * f)/t/t;
    }

    return std::norm(3.0/4.0*A);
}

double Higgs::BrH2AA(const double &Q) const {
    return BrHGrid.at(HiggsDecay::AA).Interpolate(Q)/BrHGrid.at(HiggsDecay::Width).Interpolate(Q);
}

void Higgs::MakeEvent(Event &event, const std::vector<FourVector>& momenta) {
    std::vector<Particle> particles;
    switch(higgsDecay) {
        case HiggsDecay::AA:
            particles.push_back(Particle(25,momenta[0],0));
            particles.push_back(Particle(22,momenta[1],2));
            particles.push_back(Particle(22,momenta[2],2));

            particles[0].SetDaughters(2,3);
            particles[1].SetMothers(1,0);
            particles[2].SetMothers(1,0);
            break;
        default:
            throw std::runtime_error("Higgs: Invalid Higgs decay mode");
    }

    for(auto particle : particles) {
        event.AddParticle(particle);
    }
}


//REGISTER(HiggsJet);

}
