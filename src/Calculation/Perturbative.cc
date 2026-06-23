#include "ResBos/Perturbative.hh"
#include "ResBos/Process.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/ThreadPool.hh"
#include "ResBos/Utility.hh"
#include "ResBos/DoubleExponential.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {
using namespace std::placeholders;
using namespace QCD;
REGISTER_CALC(Perturbative)

// Initialize the Perturbative class;
void Perturbative::Initialize(IO::Settings *s, std::shared_ptr<ResBos> r) {
    gType = GridType::Pert;
    SetPertOrder(s -> GetSettingSizet("PertOrder"));
    SetMuF(s -> GetSettingDouble("muF"));
    SetMuR(s -> GetSettingDouble("muR"));
    Calculation::Initialize(s,r);
    DLOG_F(INFO,"Perturbative Order: %zu", GetPertOrder());
}

// Driver function to get Perturbative piece
std::vector<double> Perturbative::GetCalc(const double &Q, const double &qt, const double &y) {
    const size_t nAngular = resbos -> GetProcess() -> GetNAngular();
    std::vector<double> pert(resbos -> GetProcess() -> GetChannels()*nAngular,0);
    std::vector<double> x1Result(resbos -> GetProcess() -> GetChannels()*nAngular,0);
    std::vector<double> x2Result(resbos -> GetProcess() -> GetChannels()*nAngular,0);

    double ecm = resbos -> GetECM();
    double S = ecm*ecm;

    // Ensure that the rapidity is in a physically allowed region
    if(fabs(y)>GetYMax(Q,qt)) return pert;

    double MT = sqrt(qt*qt+Q*Q);
    double T = -MT*ecm*exp(-y)+Q*Q;
    double U = -MT*ecm*exp(y)+Q*Q;

    double tauP = (MT+qt)/ecm;
    double x1Low = tauP*exp(y);
    double x2Low = tauP/exp(y);

    double x1Min = -U/(S+T-Q*Q);
    double x2Min = -T/(S+U-Q*Q);

    bool doX1 = true;
    bool doX2 = true;
    double aerr = 1e-16;
    double rerr = 1e-8;

    if(x1Low > 1.0) doX1 = false;
    if(x2Low > 1.0) doX2 = false;

    Utility::DoubleExponential integrator1{}, integrator2{};
    for(size_t i = 0; i < pert.size(); i++) {
        if(doX1) {
            auto x1Int = std::function<double(double)>(
                    std::bind(&Perturbative::X1Integral,this,_1,S,T,U,Q,qt,i));
            integrator1.SetFunction(x1Int);

            if(doX2) {
                x1Result[i] = integrator1.Integrate(x1Low,1,aerr,rerr);

                auto x2Int = std::function<double(double)>(
                        std::bind(&Perturbative::X2Integral,this,_1,S,T,U,Q,qt,i));
                integrator2.SetFunction(x2Int);
                x2Result[i] = integrator2.Integrate(x2Low,1,aerr,rerr);
            } else {
                x1Result[i] = integrator1.Integrate(x1Min,1,aerr,rerr);
            }
        } else {
            if(doX2) {
                auto x2Int = std::function<double(double)>(
                        std::bind(&Perturbative::X2Integral,this,_1,S,T,U,Q,qt,i));
                integrator2.SetFunction(x2Int);
                x2Result[i] = integrator2.Integrate(x2Min,1,aerr,rerr);
            } else {
                throw std::runtime_error("Error in Perturbative calculation. x1Low, x2Low = " + std::to_string(x1Low) + " " + std::to_string(x2Low));
            }
        }
    }

//    double coupl = resbos -> GetProcess() -> GetCoupling(GetMuR()*sqrt(Q*Q+qt*qt));
    double coupl = resbos -> GetProcess() -> GetCoupling(GetMuR()*Q);//yfu
    double alpi = resbos -> GetAlpi(GetMuR()*sqrt(Q*Q+qt*qt));

    for(size_t i = 0; i < pert.size(); i++) {
        pert[i] = hbarc2*alpi*coupl*M_PI*(x1Result[i]+x2Result[i])*2*qt;
        DLOG_F(INFO,"Pert(%zu,%f,%f,%f) = %f",i,Q,qt,y,pert[i]);
    }
            
    return pert;
}

double Perturbative::X1Integral(const double &x1, const double &S, const double &T,
                                const double &U, const double &Q, const double &qt,
                                const size_t &mode) const {
    double T1 = x1*S+U-Q*Q;
    double x2 = (-Q*Q-x1*(T-Q*Q))/T1;
    double sh = x1*x2*S;
    double th = x1*(T-Q*Q)+Q*Q;
    double uh = x2*(U-Q*Q)+Q*Q;

    return resbos -> GetProcess() -> GetPert(x1,x2,sh,th,uh,GetMuF()*sqrt(Q*Q+qt*qt),Q,qt,mode)/T1;
}

double Perturbative::X2Integral(const double &x2, const double &S, const double &T, 
                                const double &U, const double &Q, const double &qt,
                                const size_t &mode) const {
    double T2 = x2*S+T-Q*Q;
    double x1 = (-Q*Q-x2*(U-Q*Q))/T2;
    double sh = x1*x2*S;
    double th = x1*(T-Q*Q)+Q*Q;
    double uh = x2*(U-Q*Q)+Q*Q;

    return resbos -> GetProcess() -> GetPert(x1,x2,sh,th,uh,GetMuF()*sqrt(Q*Q+qt*qt),Q,qt,mode)/T2;
}

}
