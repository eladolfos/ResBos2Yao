#include "ResBos/Electroweak.hh"
#include "ResBos/Process.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Resummation.hh"
#include "ResBos/Settings.hh"
#include "ResBos/ThreadPool.hh"
#include "ResBos/Utility.hh"
#include "ResBos/DoubleExponential.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {
    using namespace std::placeholders;
    REGISTER_CALC(Resummation)

    // Give initial values to bMax and Q0
    double Resummation::bMax = 0.5;
    double Resummation::Q0 = 1.6;
    double Resummation::aerr = 1e-8;
    double Resummation::rerr = 1e-16;

    // Initialize the Resummation class
    void Resummation::Initialize(IO::Settings *s, std::shared_ptr<ResBos> r) {
        gType = GridType::Resum;
        SetAOrder(s->GetSettingSizet("AOrder"));
        SetBOrder(s->GetSettingSizet("BOrder"));
        SetCOrder(s->GetSettingSizet("COrder"));
        SetHOrder(s->GetSettingSizet("HOrder"));
        Q0 = s->GetSettingDouble("Q0");
        bMax = s->GetSettingDouble("bMax");
        SetC1(s->GetSettingDouble("C1")*QCD::B0);
        SetC2(s->GetSettingDouble("C2"));
        SetC3(s->GetSettingDouble("C3")*QCD::B0);
        if(GetScheme() == Scheme::CFG) {
            SetMuR(s->GetSettingDouble("muR"));
        }
        
        std::string gees = s -> GetSettingString("g");
        std::stringstream ss(gees);
        double i;
        while(ss>>i) {
            g.push_back(i);
            if(ss.peek() == ',' || ss.peek() == ' ') ss.ignore();
        }

        std::string nonPertStr = s->GetSettingString("NonPert");
        if(nonPertStr.compare("Gaussian") == 0) iNonPert = NonPertEnum::Gaussian;
        else if(nonPertStr.compare("BLNY") == 0) iNonPert = NonPertEnum::BLNY;
        else if(nonPertStr.compare("SIYY") == 0) iNonPert = NonPertEnum::SIYY;
        else if(nonPertStr.compare("SIYYgy") == 0) iNonPert = NonPertEnum::SIYYgy;
        else if(nonPertStr.compare("IY") == 0) iNonPert = NonPertEnum::IY;
        else if(nonPertStr.compare("IY1") == 0) iNonPert = NonPertEnum::IY1;
        else if(nonPertStr.compare("IY2") == 0) iNonPert = NonPertEnum::IY2;
        else if(nonPertStr.compare("IY6") == 0) iNonPert = NonPertEnum::IY6;
        else if(nonPertStr.compare("TMD") == 0) iNonPert = NonPertEnum::TMD;
        else if(nonPertStr.compare("None") == 0) iNonPert = NonPertEnum::None;
        else throw std::runtime_error("Process: Invalid Non-pert function choice. Valid options can be found in the manual");
        Calculation::Initialize(s,r);
        DLOG_F(INFO,"Resummation A Order: %zu", GetAOrder());
        DLOG_F(INFO,"Resummation B Order: %zu", GetBOrder());
        DLOG_F(INFO,"Resummation C Order: %zu", GetCOrder());
        if(GetScheme() == Scheme::CFG) {
            DLOG_F(INFO,"Resummation H Order: %zu", GetHOrder());
        }
        DLOG_F(INFO,"Resummation NonPert: %s", nonPertStr.c_str());
        std::cout << QCD::beta2(5) << std::endl;
        std::cout << GetA2(5, QCD::B0) << " " << GetA2(5, 2*QCD::B0) << " " << GetA2(5, 0.5*QCD::B0) << std::endl;
        std::cout << GetA3(5, QCD::B0) << " " << GetA3(5, 2*QCD::B0) << " " << GetA3(5, 0.5*QCD::B0) << std::endl;
        std::cout << GetA4(5, QCD::B0) << " " << GetA4(5, 2*QCD::B0) << " " << GetA4(5, 0.5*QCD::B0) << std::endl;
        // throw;
    }

    // Test function for obtaining b-space
    void Resummation::BSpacePlot(const double &Q, const double &y) {
        double ecm = resbos -> GetECM();

        double x1, x2;
        x1 = Q/ecm*exp(y);
        x2 = Q/ecm*exp(-y);

        // Ensure that the partonic fractions are in a physically allowed region
        if(x1 > 1 || x2 > 1) return;

        // Fill bins according to benchmarking study
//        std::vector<double> binEdges;
//        for(size_t i = 0; i <= 40; ++i) binEdges.push_back(i);
//        for(size_t i = 0; i < 12; ++i) binEdges.push_back(45+i*5);

        // Fill bins for Ogata study
        std::vector<double> binEdges = Utility::Logspace(-5,2,100);

        // Define couplings using EW class
        double gvu=2*(0.5)-4*2.0/3.0*abs(resbos->GetProcess()->GetEW()->GetSW2());
        double gvd=2*(-0.5)-4*(-1.0/3.0)*abs(resbos->GetProcess()->GetEW()->GetSW2());
        double gau=-1;
        double gad=1.0;
        gvu=gvu/2.0; 
        gvd=gvd/2.0;
        gau=gau/2.0;
        gad=gad/2.0;

        double FAC = sqrt(abs(resbos->GetProcess()->GetEW()->GetZCoupl()));

        double QL_Z1 = (gvu-gau)*FAC/2.0; 
        double QR_Z1 = (gvu+gau)*FAC/2.0;
        double QL_Z2 = (gvd-gad)*FAC/2.0;
        double QR_Z2 = (gvd+gad)*FAC/2.0;

        for(size_t i = 0; i < binEdges.size()-1; ++i) {
            double binCenter = binEdges[i] + (binEdges[i+1]-binEdges[i])/2.0;
            double bspaceZU = FTransform(binCenter, Q, x1, x2, 0);
            double bspaceZD = FTransform(binCenter, Q, x1, x2, 2);

            double bspace = bspaceZU*(QL_Z1*QL_Z1+QR_Z1*QR_Z1)
                          + bspaceZD*(QL_Z2*QL_Z2+QR_Z2*QR_Z2);

            std::cout << binCenter << " " << bspace << std::endl;
        }
    }

    // Driver function to get Resummation piece
    std::vector<double> Resummation::GetCalc(const double &Q, const double &qt, const double &y) {
        std::vector<double> resum(resbos -> GetProcess() -> GetChannels() * resbos -> GetProcess() -> GetResumAngular());

        // Ensure that the rapidity is in a physically allowed region
        if(fabs(y)>GetYMax(Q,qt)) return resum;

        // Calculate the partonic fractions
        const double ecm = resbos -> GetECM();
        double x1, x2;
        if(!KinCorr) {
            x1 = Q/ecm*exp(y);
            x2 = Q/ecm*exp(-y);
        } else {
            x1 = sqrt(Q*Q+qt*qt)/ecm*exp(y);
            x2 = sqrt(Q*Q+qt*qt)/ecm*exp(-y);
        }

        // Ensure that the partonic fractions are in a physically allowed region
        if(x1 > 1 || x2 > 1) return resum;

        // Perform Hankel Transform using Ogata integration
        auto FTransformFunc = std::function<std::vector<double>(double)>(std::bind(&Resummation::FTransformVec,this,_1,Q,x1,x2));
        std::vector<double> resTot(resum.size());
        auto bRange = std::make_pair(0, 10);
        constexpr double ogata_rerr = 1e-4;
        constexpr double ogata_aerr = 1e-8;
        resTot = ogata.FBT(FTransformFunc, qt, y, bRange, ogata_rerr, ogata_aerr);
        double HardPart = 1.0;
        if(GetScheme() == Scheme::CFG) HardPart = resbos -> H(GetHOrder(),Q,GetMuR()*Q);
        const double coupl = resbos -> GetProcess() -> GetCoupling(Q);
        for(size_t resumMode = 0; resumMode < resum.size(); resumMode++) {
            // Obtain the result in terms of pb instead of 1/GeV^2
            resum[resumMode] = resTot[resumMode]*coupl*QCD::hbarc2/pow(ecm,2)*HardPart*qt;
            DLOG_F(INFO,"Resummed(%zu,%f,%f,%f) = %f", resumMode,Q,qt,y,resum[resumMode]);
        }

        return resum;
    }

    std::vector<Conv> Resummation::GetNeededConvs() const{
        std::vector<Conv> neededGrids;
        if(GetCOrder()>0) {
            neededGrids.push_back(Conv::C1);
            if(GetCOrder()>1) {
                neededGrids.push_back(Conv::C2);
            }
        }
        return neededGrids;
    }

//    // The integrand to be Fourier Transformed returning a vector for scale or PDF variation
//    std::vector<double> Resummation::FTransformVecVar(double eta, double Q, double qt, double x1, double x2, Variation* var) {
//        std::vector<double> result = {0,0,0,0};
//        double b = eta/qt;
//        double nonPert = NonPert(b,Q,x1,x2);
//        if(nonPert < 1E-16) return result;
//
//        double sud = SudakovVar(b,Q,var);
//        std::vector<double> cfcf = CxFCxFVecVar(b,x1,x2,var);
//        for(int i = 0; i < result.size(); i++) result[i] = eta*sud*cfcf[i]*nonPert*Utility::BesJ0(eta);
//        return result;
//    }

    // The integrand to be Fourier Transformed
    double Resummation::FTransform(const double &b, const double &Q, const double &x1,
                                   const double &x2, const size_t &mode) const {

        double nonPert = NonPert(b,Q,x1,x2);
        if(nonPert < 1E-16) return 0;

        double mu = GetC3()/GetBStar(b);
// TODO: This is something that needs to be decided on, maybe add a flag to use this option?
//        if(mu < resbos -> GetBeams().first -> GetQMin()) {
//            mu = GetC3()/GetBStar(b,GetC3()/resbos -> GetBeams().first -> GetQMin());
//        }

        double sud = Sudakov(b,Q);
        if(sud < 1E-8) return 0;
        double cfcf = resbos -> GetProcess() -> CxFCxF(mu,x1,x2,mode);
        return b*sud*cfcf*nonPert;
    }

#ifndef FITTING
    // The integrand to be Fourier Transformed
    std::vector<double> Resummation::FTransformVec(const double &b, const double &Q,
                                                   const double &x1, const double &x2) const {
        std::vector<double> result(resbos -> GetProcess() -> GetChannels() 
                                 * resbos -> GetProcess() -> GetResumAngular(),0);
        const double nonPert = NonPert(b,Q,x1,x2);
        if(nonPert < 1E-16) return result;

        const double mu = GetC3()/GetBStar(b); //yfu
// TODO: This is something that needs to be decided on, maybe add a flag to use this option?
//        if(mu < resbos -> GetBeams().first -> GetQMin()) {
//            mu = GetC3()/GetBStar(b,GetC3()/resbos -> GetBeams().first -> GetQMin());
//        }

        const double sud = Sudakov(b,Q);
        if(sud < 1E-8) return result;
        for(size_t i = 0; i < result.size(); ++i) {
            const double cfcf = resbos -> GetProcess() -> CxFCxF(mu,x1,x2,i);
            result[i] = b*sud*cfcf*nonPert;
        }
        return result;
    }
#else
    // The integrand to be Fourier Transformed
    std::vector<double> Resummation::FTransformVec(const double &b, const double &Q,
                                                   const double &x1, const double &x2) {
        std::vector<double> result(resbos -> GetProcess() -> GetChannels() 
                                 * resbos -> GetProcess() -> GetResumAngular(),0);
        const double nonPert = NonPert(b,Q,x1,x2);
        if(nonPert < 1E-16) return result;

        double y = 0.5*log(x1/x2); 
        auto it = SudPertMap.find({b,Q,y});
        if(it != SudPertMap.end()) {
            for(size_t i = 0; i < result.size(); ++i) {
                result[i] = (*it).second[i]*nonPert;
            }
            return result;
        } else {
            const double mu = GetC3()/GetBStar(b); //yfu

            const double sud = Sudakov(b,Q);
            std::vector<double> store(result.size());
            for(size_t i = 0; i < result.size(); ++i) {
                const double cfcf = resbos -> GetProcess() -> CxFCxF(mu,x1,x2,i);
                result[i] = b*sud*cfcf*nonPert;
                store[i] = b*sud*cfcf;
            }
            SudPertMap[{b,Q,y}] = store;
            return result;
        }
    }
#endif

    // Calculation of Non-Pert functions. To add a new function, make sure to also add it to Enums.hh
    double Resummation::NonPert(const double &b, const double &Q, 
            const double &x1, const double &x2) const {
        const double b2 = b*b;
        //static constexpr double x0 = 0.01; //lambda = 0.2;
        const double lambda = g[3];
        const double x0 = g[4];
        const double y = 0.5*log(x1/x2);  // IncludeY() ? 0.5*log(x1/x2) : 0; 
        const double yMax = 5.0; // GetYMax(Q);
        const double ecm = resbos -> GetECM();
        const double MZ = resbos -> GetProcess() -> GetEW() -> GetMzReal();
        double exponent = 0;
        switch(iNonPert) {
            case NonPertEnum::Gaussian:
                exponent = -b2*g[0];
                break;
            case NonPertEnum::BLNY:
                exponent = -b2*(g[0]+g[2]*log(100*x1*x2)+g[1]*log(Q/2.0/Q0));
                break;
            case NonPertEnum::SIYY:
                exponent = -(b2*(g[0]+g[2]*(pow(x0/x1,lambda)+pow(x0/x2,lambda)))+g[1]*log(b/GetBStar(b))*log(Q/Q0));
                break;
            case NonPertEnum::SIYYgy:
                exponent = (-b2*(g[0]+g[2]*(pow(x0/x1,lambda)+pow(x0/x2,lambda))+g[1]*log(Q/2.0/Q0)));
                break;
            case NonPertEnum::IY:
                exponent = (-b2*(g[0]+(g[1]+g[2]*b2)*log(Q/2.0/Q0)+g[3]*log(100*x1*x2))*(0.5-0.5*tanh(g[4]*(y-g[5]))));
                break;
            case NonPertEnum::IY1:
                exponent = (-b2*(g[0]+(g[1]+g[2]*b2)*log(Q/2.0/Q0)+g[3]*log(100*x1*x2)+g[4]*tanh(g[5]*(fabs(y)-yMax))));
                /*
                IY1 = ((g[0]+g[4]*tanh(g[5]*(fabs(y)-yMax))));
                IY4 = ((g[0]+g[4]+g[4]*tanh(g[5]*(fabs(y)-yMax)))));
                */
                break;
            case NonPertEnum::IY2:
                exponent = (-b2*(g[0]+(g[1]+g[2]*b2)*log(Q/2.0/Q0)+g[3]*log(100*x1*x2)+g[4]*fabs(y)/yMax));
                break;
            case NonPertEnum::IY6:
                exponent = -b2*(g[0]+(g[1]+g[2]*b2)*log(Q/MZ)+g[3]*log(1960/ecm)+g[4]*(tanh(g[5]*yMax)+tanh(g[5]*(fabs(y)-yMax))));
                break;
            case NonPertEnum::TMD:
                exponent = -(g[0]*(1-x1)+g[1]*x1+g[2]*x1*(1-x1))*b2/sqrt(1+g[3]*pow(x1, g[4])*b2)
                           -(g[0]*(1-x2)+g[1]*x2+g[2]*x2*(1-x2))*b2/sqrt(1+g[3]*pow(x2, g[4])*b2);
                break;
            case NonPertEnum::None:
                exponent = 0.0;
                break;
            default:
                throw std::runtime_error("Resummation: Invalid NonPert Function");
        }
        if(exponent > 10)
            throw std::runtime_error("Invalid non-pert params");
        return exp(exponent);
    }

    // Calculation of the perturbative Sudakov factor
    double Resummation::Sudakov(const double &b, const double &Q) const {
        const double bStar = GetBStar(b);

        const double uplim = 2*log(GetC2()*Q);
        const double dnlim = 2*log(GetC1()/bStar);

        auto SudFunc = std::function<double(double)>(std::bind(&Resummation::SudInt,this,_1,Q));
        Utility::DoubleExponential SudIntegral(SudFunc);
        if(uplim > dnlim) return exp(-SudIntegral.Integrate(dnlim,uplim,aerr,rerr));
        else return exp(SudIntegral.Integrate(uplim,dnlim,aerr,rerr));
    }

//    // Calculation of the perturbative Sudakov factor
//    double Resummation::SudakovVar(double b, double Q, Variation* var) {
//        double bStar = GetBStar(b);
//
//        double uplim = 2*log(var -> GetC2()*Q);
//        double dnlim = 2*log(var -> GetC1()/bStar);
//
//        auto SudFunc = std::function<double(double)>(std::bind(&Resummation::SudIntVar,this,_1,Q,var));
//        Utility::Integrate SudIntegral(SudFunc);
//        if(uplim > dnlim) return exp(-SudIntegral.Adzint(dnlim,uplim,1E-8,1E-4,2,2));
//        else return exp(SudIntegral.Adzint(uplim,dnlim,1E-16,1E-8,2,2));
//    }

    // Calculation of the perturbative Sudakov integrand
    double Resummation::SudInt(const double &logMu2, const double &Q) const {
        const double mu = sqrt(exp(logMu2));
        const double alpi = resbos -> GetAlpi(mu);
        const size_t nf = resbos -> GetNF(mu);

        const double sQ = GetC2()*Q;
        const double dlQ = log(sQ*sQ)-logMu2;
        return GetA(nf,alpi,GetC1())*dlQ+GetB(nf,alpi,GetC1(),GetC2());
    }

//    // Calculation of the perturbative Sudakov integrand
//    double Resummation::SudIntVar(double logMu2, double Q, Variation *var) {
//        double mu = sqrt(exp(logMu2));
//        double alpi = process -> pdf -> Alpi(mu);
//        int nf = process -> pdf -> NF(mu);
//
//        double sQ = var -> GetC2()*Q;
//        double dlQ = log(sQ*sQ)-logMu2;
//        return GetA(nf,alpi,var -> GetC1())*dlQ+GetB(nf,alpi,var -> GetC1(), var -> GetC2());
//    }

    double Resummation::GetA(const size_t &nf, const double &alpi, const double &C1_) const {
        switch(GetAOrder()) {
            case 1:
                return alpi*GetA1();
            case 2:
                return alpi*(GetA1()+alpi*GetA2(nf,C1_));
            case 3:
                return alpi*(GetA1()+alpi*(GetA2(nf,C1_)+alpi*GetA3(nf,C1_)));
            case 4:
                return alpi*(GetA1()+alpi*(GetA2(nf,C1_)+alpi*(GetA3(nf,C1_)+alpi*GetA4(nf,C1_))));
            case 5:
                return alpi*(GetA1()+alpi*(GetA2(nf,C1_)+alpi*(GetA3(nf,C1_)
                                +alpi*(GetA4(nf,C1_)+alpi*GetA5(nf,C1_)))));
        }
        return 0;
    }

    double Resummation::GetB(const size_t &nf, const double &alpi,
                             const double &C1_, const double &C2_) const {
        switch(GetBOrder()) {
            case 1:
                return alpi*GetB1(nf,C1_,C2_);
            case 2:
                return alpi*(GetB1(nf,C1_,C2_)+alpi*GetB2(nf,C1_,C2_));
            case 3:
                return alpi*(GetB1(nf,C1_,C2_)+alpi*(GetB2(nf,C1_,C2_)+alpi*GetB3(nf,C1_,C2_)));
        }
        return 0;
    }

}
