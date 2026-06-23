#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <memory>

#include "ResBos/BaseIntegrator.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/DoubleExponential.hh"
#include "ResBos/Grid2D.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/loguru.hpp"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"
#include "ResBos/ProgressBar.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/ThreadPool.hh"
#include "ResBos/Utility.hh"

#ifdef USING_FFTW
#include "fftwpp/Complex.h"
#include "fftwpp/fftw++.h"
#include "fftwpp/convolution.h"
#endif

#ifdef HAVE_GZIP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "gzstream/gzstream.h"
#pragma GCC diagnostic pop
#endif

// Store information about the ThreadPool
ThreadPool *pool;

namespace Beam {
    // Convolution class constructor
    Convolution::Convolution(std::shared_ptr<ResBos::ResBos> resbos_, Utility::PDF* pdf_, 
            Utility::Hoppet* hoppet_,
            const bool &gridGen_, const size_t &COrder_, const Scheme &scheme_,
            const double &C1_, const double &C2_, const double &C3_) :
        gridGen(gridGen_), COrder(COrder_),
        scheme(scheme_) {

        // Initialize class objects
        resbos = resbos_; 
        pdf = pdf_;
        hoppet = hoppet_;

        // Initialize C values for scale dependence
        C1 = C1_;
        C2 = C2_;
        C3 = C3_;

        // Initialize the vectors for the grids in x and Q space
        for(int i = N; i >= 0; --i) {
            xVec.push_back(exp(-uMin-i*uStep));
        }

        qVec.push_back(pdf -> qMin());
        for(unsigned int j = 0; j < nQ-1; ++j) {
            qVec.push_back(qBase*exp(exp(t0+j*dtt)));
        }

        m_integrator = std::unique_ptr<Utility::BaseIntegrator>(
                new Utility::DoubleExponential());
    }

    // Destructor
    Convolution::~Convolution() {}


    // Get the Convolution (conv) for a given pid, x and Q value
    // Process is passed to calculate scheme dependence
    double Convolution::GetConv(const int &pid, const double &x, 
                                const double &Q, const Conv &conv) {
        double schemeDep = 0;
        switch(conv) {
            case Conv::C:
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return Cf.at(pid) -> Interpolate(x,Q)/x;
                return CConv(pid, x, Q);
            case Conv::C1:
                if(scheme == Scheme::CSS) schemeDep = SchemeDependence(pid,x,Q,Conv::C1);
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return C1f.at(pid) -> Interpolate(x,Q)/x+schemeDep;
                return C1Conv(pid, x, Q)+schemeDep;
            case Conv::C2:
                if(scheme == Scheme::CSS) schemeDep = SchemeDependence(pid,x,Q,Conv::C2);
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return C2f.at(pid) -> Interpolate(x,Q)/x+schemeDep;
                return C2Conv(pid, x, Q)+schemeDep;
            case Conv::C1P1:
                if(scheme == Scheme::CSS) schemeDep = SchemeDependence(pid,x,Q,Conv::C1P1);
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return C1P1f.at(pid) -> Interpolate(x,Q)/x+schemeDep;
                return C1Conv(pid, x, Q, 1)+schemeDep;
            case Conv::C1P1P1:
                if(scheme == Scheme::CSS) schemeDep = SchemeDependence(pid,x,Q,Conv::C1P1P1);
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return C1P1P1f.at(pid) -> Interpolate(x,Q)/x+schemeDep;
                return C1Conv(pid, x, Q, 11)+schemeDep;
            case Conv::C1P2:
                if(scheme == Scheme::CSS) schemeDep = SchemeDependence(pid,x,Q,Conv::C1P2);
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return C1P2f.at(pid) -> Interpolate(x,Q)/x+schemeDep;
                return C1Conv(pid, x, Q, 2)+schemeDep;
            case Conv::C2P1:
                if(scheme == Scheme::CSS) schemeDep = SchemeDependence(pid,x,Q,Conv::C2P1);
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return C2P1f.at(pid) -> Interpolate(x,Q)/x+schemeDep;
                return C2Conv(pid, x, Q, 1)+schemeDep;
            case Conv::G1:
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return G1f.at(pid) -> Interpolate(x,Q)/x;
                return G1Conv(pid,x,Q);
            case Conv::G1P1:
                if(gridGen && Q < qVec.back() && Q > qVec.at(0)) return G1P1f.at(pid) -> Interpolate(x,Q)/x;
                return G1Conv(pid,x,Q,1);
        }
        return 0;
    }

    // Get the Convolution (conv) for a given pid, x and Q value
    // Process is passed to calculate scheme dependence
    std::map<int,double> Convolution::GetConv(const double &x, const double &Q,
                                              const Conv &conv) {
        std::map<int,double> result;
        for(int i = -5; i < 6; ++i) {
            result[i] = GetConv(i,x,Q,conv);
        }

        return result;
    }

    // Helper function to generate Convolution grids
    void Convolution::GenerateGrid(PDFThreads pdfs_, Utility::Hoppet *hoppet_, Conv conv){
        pdf = pdfs_[mainThread];
        pdfs = pdfs_;
        hoppet = hoppet_;

        GenerateGrid(conv);
    }

    // Calculate the requested convolution
    void Convolution::GenerateGrid(const Conv &conv) {
        if(hoppet==NULL) 
            throw std::runtime_error("Convolution: Called grid generation without first setting Hoppet");
        if(pdf==NULL) 
            throw std::runtime_error("Convolution: Called grid generation without first setting PDF");
        for(auto pdfi : pdfs)
            if(pdfi.second==NULL) 
                throw std::runtime_error("Convolution: Called grid generation without first setting threaded PDFs");

        switch(conv) {
            case Conv::C:
                CalcCGrid();
                break;
            case Conv::C1:
                C1f = CalcC1Grid();
                break;
            case Conv::C2:
                C2f = CalcC2Grid();
                break;
            case Conv::C1P1:
                C1P1f = CalcC1Grid(1);
                break;
            case Conv::C1P1P1:
                C1P1P1f = CalcC1Grid(11);
                break;
            case Conv::C1P2:
                C1P2f = CalcC1Grid(2);
                break;
            case Conv::C2P1:
                C2P1f = CalcC2Grid(1);
                break;
            case Conv::G1:
                G1f = CalcG1Grid();
                break;
            case Conv::G1P1:
                G1P1f = CalcG1Grid(1);
                break;
            default:
                throw std::runtime_error("Conv: Invalid convolution requested");
        }
    }

    using namespace std::placeholders;

    // Calculate the CxF that appears inside the b-space integration
    void Convolution::CalcCGrid() {
        using namespace QCD;

        std::cout << "Initializing CxF..." << std::endl;
        LOG_F(INFO,"Begin initializing CxF");

        //Initialize the Grid2D objects
        for(int pid = -5; pid < 6; pid++) {
            Cf[pid] = new Utility::Grid2D(xVec,qVec);
        }

        ProgressBar pb(N*nQ*6);

        //Calculate the convolutions
        for(size_t iX = 0; iX <= N; iX++) {
            const double x = Cf.at(1) -> GetX(iX);
            DLOG_F(INFO,"Evaluating for x = %f",x);
            for(size_t iQ = 0; iQ < nQ; iQ++) {
                const double mu = Cf.at(1) -> GetY(iQ);
                const size_t nf = pdf -> NF(mu);
                for(int pid = 0; pid < 6; pid++) {
                    if(pid > static_cast<int>(nf) || x == 1) {
                        Cf.at(pid) -> AddPoint(x,mu,0);
                        Cf.at(-pid) -> AddPoint(x,mu,0);
                        continue;
                    }
                    const double fpdf = pdf -> Apdf(pid,x,mu);
                    double conv = fpdf*x;
                    const double alpi = pdf -> Alpi(mu);
                    if(COrder>0) {
                        conv += alpi*C1f[pid] -> FindPoint(iX,iQ);
                        if(scheme == Scheme::CSS) conv += alpi*SchemeDependence(pid,x,mu,Conv::C1)*x;
                        conv += ScaleVariation(1,pid,x,mu)*x;
                        if(COrder>1) {
                            conv += pow(alpi,2)*C2f[pid] -> FindPoint(iX, iQ);
                            if(scheme == Scheme::CSS) conv += pow(alpi,2)*SchemeDependence(pid,x,mu,Conv::C2)*x;
                            conv += ScaleVariation(2,pid,x,mu)*x;
                        }
                    }
                    Cf.at(pid) -> AddPoint(x,mu,conv);
                    if(pid == 1 || pid == 2) {
                        double fpdf2 = pdf -> Apdf(-pid,x,mu);
                        double conv2 = fpdf2*x;
                        if(COrder>0) {
                            conv2 += alpi*C1f[-pid] -> FindPoint(iX, iQ);
                            if(scheme == Scheme::CSS) conv2 += alpi*SchemeDependence(-pid,x,mu,Conv::C1)*x;
                            conv2 += ScaleVariation(1,-pid,x,mu)*x;
                            if(COrder>1) {
                                conv2 +=pow(alpi,2)*C2f[-pid] -> FindPoint(iX, iQ);
                                if(scheme == Scheme::CSS) conv2 += pow(alpi,2)*SchemeDependence(-pid,x,mu,Conv::C2)*x;
                                conv2 += ScaleVariation(2,-pid,x,mu)*x;
                            }
                        }
                        Cf.at(-pid) -> AddPoint(x,mu,conv2);
                    } else if(pid != 0) {
                        Cf.at(-pid) -> AddPoint(x,mu,conv);
                    }
                    pb.Update(1);
                    pb.Display();
                }
            }
        }
        pb.Done();
        std::cout << "Finished generating CxF grid" << std::endl;
        LOG_F(INFO,"Finished generating CxF grid");
    }

    // Calculate Scale Variations for the resummed CxF piece
    double Convolution::ScaleVariation(int scaleOrder, int pid, double x, double mu) {
        using namespace QCD;
        double conv=0;
        static double fpdf, A1c, B1c, alpi; 
        static size_t nf; 
        if(C2*B0 == C1 && C3 == B0) return 0;
        static const double logC3B0 = 2*log(C3/B0);
        static const double logB0C2C1 = 2*log(B0*C2/C1);
        if(scaleOrder==1) {
            fpdf = pdf->Apdf(pid,x,mu);
            nf = pdf -> NF(mu);
            A1c = resbos -> GetCalculation() -> GetA1();
            B1c = resbos -> GetCalculation() -> GetB1(nf);
            alpi = pdf->Alpi(mu);
            if(C2*B0 != C1) {
                conv += (-1.0/4.0*A1c*pow(logB0C2C1,2)
                        +0.5*B1c*logB0C2C1)*fpdf;
            }
            if(C3 != B0) {
                std::map<int,double> hoppetOut = hoppet->GetConvolution(x,mu,1);
                conv -= hoppetOut[pid]*log(C3/B0);
            }
            DLOG_F(3,"Scale Variation: C1 = %f, C2 = %f, C3 = %f", C1, C2, C3);
            DLOG_F(3,"Scale Variation: x = %f, pid = %d, mu = %f",x,pid,mu);
            DLOG_F(3,"Scale Variation: result(alphas) = %e", conv*alpi);
            return conv*alpi;
        } else if(scaleOrder==2) {
            static double A2c, B2c, logC3B0, C1Val, H1 = 0;
            if(C2*B0 != C1) {
                A2c = resbos -> GetCalculation() -> GetA2(nf);
                B2c = resbos -> GetCalculation() -> GetB2(nf);
                if(scheme == Scheme::CSS) H1 = resbos -> GetProcess() -> H1();
                C1Val = C1Conv(pid,x,mu)+0.5*H1*fpdf;
                conv += (-1.0/4.0*beta0(nf)*A1c*pow(logB0C2C1,2)*logC3B0
                        + 0.5*beta0(nf)*B1c*logB0C2C1*logC3B0+1.0/32.0*pow(A1c,2)*pow(logB0C2C1,4)
                        - 1.0/12.0*beta0(nf)*A1c*pow(logB0C2C1,3)-1.0/8.0*A1c*B1c*pow(logB0C2C1,3)
                        + 0.5*B2c*logB0C2C1-0.25*A2c*pow(logB0C2C1,2)+1.0/8.0*pow(B1c*logB0C2C1,2)
                        + 0.25*beta0(nf)*B1c*pow(logB0C2C1,2))*fpdf
                    + (0.5*B1c*logB0C2C1-0.25*A1c*pow(logB0C2C1,2))*C1Val;
            }
            if(C3 != B0) {
                std::map<int,double> P1f = hoppet->GetConvolution(x,mu,1);
                std::map<int,double> P1P1f = hoppet->GetConvolution(x,mu,1,1);
                std::map<int,double> P2f = hoppet->GetConvolution(x,mu,2);
                C1Val = C1Conv(pid,x,mu)+0.5*H1*fpdf;
                double C1P1Val = C1Conv(pid,x,mu,1)+0.5*H1*P1f[pid];
                conv += (beta0(nf)*C1Val-0.5*C1P1Val-0.25*P2f[pid])*logC3B0
                    + 1.0/8.0*P1P1f[pid]*pow(logC3B0,2)-0.25*B1c*P1f[pid]*logB0C2C1*logC3B0
                    + 1.0/8.0*A1c*P1f[pid]*pow(logB0C2C1,2)*logC3B0-beta0(nf)/4*P1f[pid]*pow(logC3B0,2);
            }
            DLOG_F(3,"Scale Variation: C1 = %f, C2 = %f, C3 = %f",C1,C2,C3);
            DLOG_F(3,"Scale Variation: x = %f, pid = %d, mu = %f",x,pid,mu);
            DLOG_F(3,"Scale Variation: result(alphas^2) = %f",conv*pow(alpi,2));
            return conv*pow(alpi,2);
        }
        return 0;
    }
    
    // Convolution calculation for C1xF using default integration techniques
    std::map<int,Utility::Grid2D*> Convolution::CalcC1Grid(const int &mode) {
        switch(mode) {
            case 0:
                std::cout << "Initializing C1xF... ";
                LOG_F(INFO,"Begin initializing C1xF");
                break;
            case 1:
                std::cout << "Initializing C1xP1xF... ";
                LOG_F(INFO,"Begin initializing C1xP1xF");
                break;
            case 2:
                std::cout << "Initializing C1xP2xF... ";
                LOG_F(INFO,"Begin initializing C1xP2xF");
                break;
            case 11:
                std::cout << "Initializing C1xP1xP1xF... ";
                LOG_F(INFO,"Begin initializing C1xP1xP1xF");
                break;
        }
        std::cout << std::endl;

        std::map<int,Utility::Grid2D*> PDFMap;
        for(int pid=-5; pid<=5; ++pid) {
            PDFMap[pid] = new Utility::Grid2D(xVec,qVec);
        }

        ProgressBar pb(N*nQ*6);
        for(size_t iQ = 0; iQ < nQ; ++iQ) {
            auto t1 = std::chrono::high_resolution_clock::now();
            const double Q = qVec[iQ];
            const size_t nf = pdf -> NF(Q);
            for(int pid = 0; pid <= 5; ++pid) {
                for(size_t i = 0; i < N; ++i) {
                    double x = xVec[i];
                    if(pid <= static_cast<int>(nf)) {
                        double test_quark = C1Conv(pid, x, Q, mode, mainThread);
                        PDFMap[pid] -> AddPoint(i,iQ,test_quark*x);
                        if(pid == 1 || pid == 2) {
                            test_quark = C1Conv(-pid, x, Q, mode, mainThread);
                            PDFMap[-pid] -> AddPoint(i,iQ,test_quark*x);
                        } else if(pid != 0) {
                            PDFMap[-pid] -> AddPoint(i,iQ,test_quark*x);
                        }
                    } else {
                        PDFMap[pid] -> AddPoint(i,iQ,0);
                        PDFMap[-pid] -> AddPoint(i,iQ,0);
                    }
                    pb.Update(1);
                    pb.Display();
                }
            }
            auto t5 = std::chrono::high_resolution_clock::now();
            auto pidTime = t5-t1;
            auto duration = std::chrono::duration<double,std::milli>(pidTime).count();
            LOG_F(INFO,"Total time for Q=%f: %f ms", Q, duration);
        }
        switch(mode) {
            case 0:
                LOG_F(INFO,"Finished initializing C1xF");
                break;
            case 1:
                LOG_F(INFO,"Finished initializing C1xP1xF");
                break;
            case 2:
                LOG_F(INFO,"Finished initializing C1xP2xF");
                break;
            case 11:
                LOG_F(INFO,"Finished initializing C1xP1xP1xF");
                break;
        }

        pb.Done();
        std::cout << "done" << std::endl;
        return PDFMap;
    }

    double Convolution::C1qq(const double& x) const {
        return 0.5*QCD::CF*(1-x);
    }

    double Convolution::C1gq(const double& x) const {
        return 0.5*QCD::CF*x;
    }

    double Convolution::C1qg(const double& x) const {
        return 0.5*x*(1-x);
    }

    double Convolution::C2qq(const double& z) const {
        using namespace QCD;
        using namespace Utility;

        double CxFQQ = CA*CF*((1.0+z*z)/(1-z)*(-xLi(3,1-z)/2.0+xLi(3,z)-xLi(2,z)*log(z)/2.0
                    -0.5*xLi(2,z)*log(1-z)-1.0/24.0*pow(log(z),3)-0.5*pow(log(1-z),2)*log(z)
                    +1.0/12.0*pi2*log(1-z)-pi2/8)+1.0/(1.0-z)*(-0.25*(11-3*z*z)*ZETA3
                    -1.0/48.0*(-z*z+12*z+11)*pow(log(z),2)-1.0/36.0*(83.0*z*z-36.0*z+29.0)
                    *log(z)+pi2*z/4.0)
                +(1-z)*(xLi(2,z)/2.0+0.5*log(1-z)*log(z))+(z+100)/27.0+0.25*z*log(1-z));
        CxFQQ += CF*CF*((1.0+z*z)/(1.0-z)*(xLi(3,1-z)/2.0+xLi(2,z)*log(1.0-z)/2.0
                    +3.0*xLi(2,z)*log(z)/2.0+3.0/4.0*log(z)*pow(log(1.0-z),2)
                    +pow(log(z),2)*log(1-z)/4.0
                    -1.0/12.0*pi2*log(1-z))
                +(1-z)*(-xLi(2,z)-1.5*log(1-z)*log(z)+2.0*pi2/3.0-29.0/4.0)
                +1.0/24.0*(1+z)*pow(log(z),3)
                +1.0/(1.0-z)*(1.0/8.0*(-2*z*z+2*z+3)*pow(log(z),2)
                    +0.25*(17*z*z-13*z+4)*log(z))-z/4.0*log(1-z));
        CxFQQ += CF*CF*((1.0+z*z)/(1.0-z)*(5*(ZETA3-xLi(3,z))/2.0));
        CxFQQ -= CF*CF/4.0*((2*pi2-18)*(1-z)-(1+z)*log(z));
        CxFQQ += CF*(1/z*(1-z)*(2*z*z-z+2)*(xLi(2,z)/6.0+1.0/6.0*log(1-z)*log(z)-pi2/36)
                +1.0/(216*z)*(1-z)*(136*z*z-143*z+172)-1.0/48.0*(8*z*z+3*z+3)*pow(log(z),2)
                +1.0/36.0*(32*z*z-30*z+21)*log(z)+1.0/24.0*(1+z)*pow(log(z),3));
        return CxFQQ;
    }

    double Convolution::C2qg(const double& z) const {
        using namespace QCD;
        using namespace Utility;

        double CxFQG = CA*(-1.0/(12*z)*(1-z)*(11*z*z-z+2)*xLi(2,1-z)
                +(2*z*z-2*z+1)*(xLi(3,1-z)/8-xLi(2,1-z)*log(1-z)/8
                    +pow(log(1-z),3)/48)
                + (2*z*z+2*z+1)*(3*xLi(3,-z)/8+xLi(3,1/(1+z))/4.0
                    -xLi(2,-z)*log(z)/8-pow(log(1+z),3)/24+pow(log(z),2)*log(1+z)/16.0
                    +pi2/48*log(1+z))
                +0.25*z*(1+z)*xLi(2,-z)+z*xLi(3,z)-0.5*z*xLi(2,1-z)*log(z)
                -z*xLi(2,z)*log(z)-3.0/8.0*(2*z*z+1)*ZETA3-149*z*z/216.0
                -(44*z*z-12*z+3)/96*pow(log(z),2)
                +(68*z*z+6*pi2*z-30*z+21)/72.0*log(z)+pi2*z/24.0+43*z/48.0+43/(108*z)
                +(2*z+1)/48.0*pow(log(z),3)-0.5*z*log(1-z)*pow(log(z),2)
                -(1-z)/8.0*z*pow(log(1-z),2)
                +0.25*z*(1+z)*log(1+z)*log(z)+(3-4*z)/16.0*z*log(1-z)-35.0/48.0);
        CxFQG += CF*((2*z*z-2*z+1)*(ZETA3-xLi(3,1-z)/8.0-xLi(3,z)/8.0
                    +xLi(2,1-z)*log(1-z)/8.0+xLi(2,z)*log(z)/8.0-pow(log(1-z),3)/48.0
                    +log(z)*pow(log(1-z),2)/16.0+pow(log(z),2)*log(1-z)/16.0)-3*z*z/8.0
                -(4*z*z-2*z+1)/96.0*pow(log(z),3)+(-8*z*z+12*z+1)/64.0*pow(log(z),2)
                +(-8*z*z+23*z+8)/32.0*log(z)+5*pi2/24.0*(1-z)*z+11*z/32.0
                +(1-z)*z/8.0*pow(log(1-z),2)-0.25*(1-z)*z*log(1-z)*log(z)
                -(3-4*z)*z/16.0*log(1-z)-9.0/32.0);
        CxFQG -= CF/4.0*(z*log(z)+0.5*(1-z*z)+(pi2-8)*z*(1-z));

        return CxFQG;     
    }

    double Convolution::C2qqb(const double& z) const {
        using namespace QCD;
        using namespace Utility;

        double CxFQQB = CF*(CF-0.5*CA)*((1+z*z)/(1+z)*(3*xLi(3,-z)/2.0+xLi(3,z)+xLi(3,1.0/(1+z))
                    -xLi(2,-z)*log(z)/2.0-xLi(2,z)*log(z)/2.0-pow(log(z),3)/24.0
                    -pow(log(1+z),3)/6.0+0.25*log(1+z)*pow(log(z),2)+pi2/12.0*log(1+z)
                    -3*ZETA3/4.0)+(1-z)*(xLi(2,z)/2.0+0.5*log(1-z)*log(z)+15.0/8.0)
                -0.5*(1+z)*(xLi(2,-z)+log(z)*log(1+z))+pi2/24.0*(z-3)+(11*z+3)/8.0*log(z));
        CxFQQB += CF*(1.0/(12*z)*(1-z)*(2*z*z-z+2)*(xLi(2,z)+log(1-z)*log(z)-pi2/6.0)
                +1.0/(432*z)*(1-z)*(136*z*z-143*z+172)-(8*z*z+3*z+3)/96.0*pow(log(z),2)
                +(32*z*z-30*z+21)/72.0*log(z)+(1+z)/48.0*pow(log(z),3));

        return CxFQQB;
    }

    double Convolution::C2qqp(const double& z) const {
        using namespace QCD;
        using namespace Utility;
        return CF*((1-z)/(12*z)*(2*z*z-z+2)*(xLi(2,z)+log(1-z)*log(z)-pi2/6)
          +(1-z)/(432*z)*(136*z*z-143*z+172)+(1+z)/48.0*pow(log(z),3)
          -(8*z*z+3*z+3)/96.0*pow(log(z),2)+(32*z*z-30*z+21)/72.0*log(z));
    }

    // Convolution calculation for C2xF using FFTs
    std::map<int, Utility::Grid2D*> Convolution::CalcC2Grid(const int &mode) {
        if(mode == 0) {
            std::cout << "Initializing C2xF...";
            LOG_F(INFO,"Begin initializing C2xF");
        } else if(mode == 1) {
            std::cout << "Initializing C2xP1xF...";
            LOG_F(INFO,"Begin initializing C2xP1xF");
        } else {
            throw std::runtime_error("Invalid convolution for C2.");
        }
        std::cout << std::endl;

        std::map<int,Utility::Grid2D*> PDFMap;
        for(int pid=-5; pid<=5; ++pid) {
            PDFMap[pid] = new Utility::Grid2D(xVec,qVec);
        }

        ProgressBar pb(N*nQ*6);
        for(size_t iQ = 0; iQ < nQ; ++iQ) {
            auto t1 = std::chrono::high_resolution_clock::now();
            const double Q = qVec[iQ];
            const size_t nf = pdf -> NF(Q);
            for(int pid = 0; pid <= 5; ++pid) {
                for(size_t i = 0; i < N; ++i) {
                    double x = xVec[i];
                    if(pid <= static_cast<int>(nf)) {
                        double test_quark = C2Conv(pid, x, Q, mode, mainThread);
                        PDFMap[pid] -> AddPoint(i,iQ,test_quark*x);
                        if(pid == 1 || pid == 2) {
                            test_quark = C2Conv(-pid, x, Q, mode, mainThread);
                            PDFMap[-pid] -> AddPoint(i,iQ,test_quark*x);
                        } else if(pid != 0) {
                            PDFMap[-pid] -> AddPoint(i,iQ,test_quark*x);
                        }
                    } else {
                        PDFMap[pid] -> AddPoint(i,iQ,0);
                        PDFMap[-pid] -> AddPoint(i,iQ,0);
                    }
                    pb.Update(1);
                    pb.Display();
                }
            }
            auto t5 = std::chrono::high_resolution_clock::now();
            auto pidTime = t5-t1;
            auto duration = std::chrono::duration<double,std::milli>(pidTime).count();
            LOG_F(INFO,"Total time for Q=%f: %f ms",Q,duration);
        }
        if(mode == 0) {
            LOG_F(INFO,"Finished initializing C2xF");
        } else if(mode == 1) {
            LOG_F(INFO,"Finished initializing C2xP1xF");
        }

        pb.Done();
        std::cout << "done" << std::endl;
        return PDFMap;
    }

    // Convolution calculation for G1xF using FFTs
    std::map<int, Utility::Grid2D*> Convolution::CalcG1Grid(const int &mode) {
        if(mode == 0) {
            std::cout << "Initializing G1xF...";
            LOG_F(INFO,"Begin initializing G1xF");
        } else if(mode == 1) {
            std::cout << "Initializing G1xP1xF...";
            LOG_F(INFO,"Begin initializing G1xP1xF");
        } else {
            throw std::runtime_error("Invalid convolution for G1.");
        }
        std::cout << std::endl;

        std::map<int,Utility::Grid2D*> PDFMap;
        for(int pid=-5; pid<=5; ++pid) {
            PDFMap[pid] = new Utility::Grid2D(xVec,qVec);
        }

        ProgressBar pb(N*nQ);
        for(size_t iQ = 0; iQ < nQ; ++iQ) {
            auto t1 = std::chrono::high_resolution_clock::now();
            const double Q = qVec[iQ];
            const int pid = 0;
            for(size_t i = 0; i < N; ++i) {
                double x = xVec[i];
                double test_quark = G1Conv(pid, x, Q, mode, mainThread);
                PDFMap[pid] -> AddPoint(i,iQ,test_quark*x);
                pb.Update(1);
                pb.Display();
            }
            auto t5 = std::chrono::high_resolution_clock::now();
            auto pidTime = t5-t1;
            auto duration = std::chrono::duration<double,std::milli>(pidTime).count();
            LOG_F(INFO,"Total time for Q=%f: %f ms",Q,duration);
        }
        if(mode == 0) {
            LOG_F(INFO,"Finished initializing C2xF");
        } else if(mode == 1) {
            LOG_F(INFO,"Finished initializing C2xP1xF");
        }

        pb.Done();
        std::cout << "done" << std::endl;
        return PDFMap;
    }

    // Function to preform the convolution integral
    double Convolution::Convolve(const double &x,
                                 const std::function<double(const double&)> &func) const {
        m_integrator -> SetFunction(func);
        double rerr = 1E-16;
        double aerr = 1E-8;
        return m_integrator -> Integrate(x, 1.0, rerr, aerr);
    }

    // Function to calculate CxF at a given x and mu
    double Convolution::CConv(const int &pid, const double &x, const double &mu,
                              const std::thread::id &id) {
        using namespace QCD;

        double conv = pdfs.at(id) -> Apdf(pid,x,mu);
        if(COrder>0) {
            double alpi = pdfs.at(id)->Alpi(mu);
            conv += alpi*GetConv(pid,x,mu,Conv::C1);
            conv += ScaleVariation(1,pid,x,mu);
            if(COrder>1) {
                conv += pow(alpi,2)*GetConv(pid,x,mu,Conv::C2);
                conv += ScaleVariation(2,pid,x,mu);
            }
        }

        return conv;
    }

    // Function to calculate C1xF in CFG 
    double Convolution::C1Conv(const int &pid, const double &x, const double &mu,
                               const int &mode, const std::thread::id &id) {
        auto fp = std::bind(&Convolution::C1xF, this, pid, x, mu, _1, mode, id);
        return Convolve(x,fp);
    }

    // Function to calculate C2xF in CFG
    double Convolution::C2Conv(const int &pid, const double &x, const double &mu,
                               const int &mode, const std::thread::id &id) {
        using namespace QCD;

        auto fp = std::bind(&Convolution::C2xF, this, pid, x, mu, _1, mode, id);
        double c = Convolve(x,fp);
        double fpdf;
        if(mode == 0) {
            fpdf = pdfs.at(id) -> Apdf(pid, x, mu);
        } else {
            std::map<int, double> hoppetOut = hoppet -> GetConvolution(x, mu, 1);
            fpdf = hoppetOut[pid];
        }
        const double nf = static_cast<double>(pdfs.at(id) -> NF(mu));
        auto plusFunc = std::bind(&Convolution::SxF, this, pid, x, mu, _1, mode, id);
        if(pid != 0) {
            return c + 0.5*((7.0*ZETA3/2.0-101.0/27.0)*CA*CF+14.0/27.0*CF*nf)
                     * (Convolve(x,plusFunc)+log(1-x)*fpdf);
        } else {
            return c + ((-101.0/27.0+7.0/2.0*ZETA3)*CA*CA+14.0/27.0*CA*nf)
                     * (Convolve(x,plusFunc)+log(1-x)*fpdf);
        }
    }

    // Function to calculate G1xF in CFG
    double Convolution::G1Conv(const int &pid, const double &x, const double &mu,
                               const int &mode, const std::thread::id &id) {
        auto fp = std::bind(&Convolution::G1xF, this, pid, x, mu, _1, mode, id);
        return Convolve(x,fp);
    }

    // Function to calculate the terms needed for CSS
    double Convolution::SchemeDependence(const int &pid, const double &x,
                                         const double &mu, const Conv &conv) const {
        double fpdf = 0, result = 0;
        std::map<int, double> hoppetOut;
        int mode = -1;
        double H1 = resbos -> GetProcess() -> H1();
        std::string name;
        switch(conv) {
            case Conv::C1: 
                fpdf = pdfs.at(mainThread) -> Apdf(pid,x,mu);
                DLOG_F(3,"Scheme Dependence C1(%d,%e,%e) = %e",pid,x,mu,0.5*H1*fpdf);
                return 0.5*H1*fpdf;
            case Conv::C1P1:
                hoppetOut = hoppet -> GetConvolution(x,mu,1);
                fpdf = hoppetOut[pid];
                DLOG_F(3,"Scheme Dependence C1P1(%d,%e,%e) = %e",pid,x,mu,0.5*H1*fpdf);
                return 0.5*H1*fpdf;
            case Conv::C1P1P1:
                hoppetOut = hoppet -> GetConvolution(x,mu,1,1);
                fpdf = hoppetOut[pid];
                DLOG_F(3,"Scheme Dependence C1P1P1(%d,%e,%e) = %e",pid,x,mu,0.5*H1*fpdf);
                return 0.5*H1*fpdf;
            case Conv::C1P2:
                hoppetOut = hoppet -> GetConvolution(x,mu,2);
                fpdf = hoppetOut[pid];
                DLOG_F(3,"Scheme Dependence C1P2(%d,%e,%e) = %e",pid,x,mu,0.5*H1*fpdf);
                return 0.5*H1*fpdf;
            case Conv::C2:
                fpdf = pdfs.at(mainThread) -> Apdf(pid,x,mu);
                result = 0.5*(resbos -> GetProcess() -> H2(mu) - 0.25*pow(H1,2))*fpdf;
                mode = 0;
                name = "C2";
                break;
            case Conv::C2P1:
                hoppetOut = hoppet -> GetConvolution(x,mu,1);
                fpdf = hoppetOut[pid];
                result = 0.5*(resbos -> GetProcess() -> H2(mu) - 0.25*pow(H1,2))*fpdf;
                mode = 1;
                name = "C2P1";
                break;
            default:
                break;
        }

        auto fp = std::bind(&Convolution::HCxF, this, pid, x, mu, _1, mode);
        result += Convolve(x,fp);
        DLOG_F(3,"Scheme Dependence %s(%d,%e,%e) = %e",name.c_str(),pid,x,mu,result);
        return result;
    }

    // Convolution of the pdf with the hard function
    double Convolution::HCxF(const int &pid, const double &x, const double &mu,
                             const double &z, const int &mode) const {
        double y = x/z, fpdf = 0, fpdfg = 0;

        // Choose whether to convolute with PDF or with Splitting Function and PDF
        // This is done to only have to code up the convolution function once to reduce typos
        double H1 = resbos -> GetProcess() -> H1();
        if(pid != 0) {
            if(mode == 0) {
                fpdf = pdfs.at(mainThread) -> Apdf(pid,z,mu);
                fpdfg = pdfs.at(mainThread) -> Apdf(0,z,mu);
            } else {
                std::map<int, double> hoppetOut = hoppet -> GetConvolution(z, mu, 1);
                fpdf = hoppetOut[pid];
                fpdfg = hoppetOut[0];
            }
            return 0.5*H1*(0.5*QCD::CF*(1-y)*fpdf+0.5*y*(1-y)*fpdfg)/z;
        } else {
            if(mode == 0) {
                fpdfg = pdfs.at(mainThread) -> Apdf(pid,z,mu);
                for(int ipid = -5; ipid < 6; ipid++) {
                    if(ipid != 0)
                        fpdf += pdfs.at(mainThread) -> Apdf(ipid,z,mu);
                }
            } else {
                std::map<int, double> hoppetOut = hoppet -> GetConvolution(z, mu, 1);
                fpdfg = hoppetOut[pid];
                for(int ipid = -5; ipid < 6; ipid++) {
                    if(ipid != 0)
                        fpdf += hoppetOut[ipid];
                }
            }
            return 0.5*H1*0.5*QCD::CF*y*fpdf/z;
        }
    }

    // Convolution of the pdf with C1
    double Convolution::C1xF(int pid, double x, double mu, double z, int mode, std::thread::id id) const {
        double fpdf, fpdfg;
        // Choose whether to convolute with PDF or with Splitting Function and PDF
        // This is done to only have to code up the convolution function once to reduce typos
        if(pid != 0) {
            if(mode == 0) {
                fpdf = pdfs.at(id) -> Apdf(pid, z, mu);
                fpdfg = pdfs.at(id) -> Apdf(0,z,mu);
            } else {
                int j = mode/10;
                int i = mode%10;
                std::map<int, double> hoppetOut = hoppet -> GetConvolution(z, mu, i, j);
                fpdf = hoppetOut[pid];
                fpdfg = hoppetOut[0];
            }
        } else { 
            fpdf = 0;
            if(mode == 0) {
                fpdfg = pdfs.at(id) -> Apdf(pid,z,mu);
                for(int ipid = -5; ipid < 6; ipid++) {
                    if(ipid == 0) continue;
                    fpdf += pdfs.at(id) -> Apdf(ipid,z,mu);
                }
            } else {
                int j = mode/10;
                int i = mode%10;
                std::map<int, double> hoppetOut = hoppet -> GetConvolution(z, mu, i, j);
                fpdfg = hoppetOut[pid];
                for(int ipid = -5; ipid < 6; ipid++) {
                    if(ipid == 0) continue;
                    fpdf += hoppetOut[ipid];
                }
            }
        }

        double y = x/z;

        if(pid != 0) return (0.5*QCD::CF*(1-y)*fpdf+0.5*y*(1-y)*fpdfg)/z;
        else return (0.5*QCD::CF*y*fpdf)/z;
    }

    // Convolution of the pdf with C2
    double Convolution::C2xF(int pid, double x, double mu, double z, int mode, std::thread::id id) {
        using namespace QCD;
        using namespace Utility;
        double fpdf=0, fpdfg=0, fpdfqb=0, fpdfqp=0;
        const int nfEff = static_cast<int>(pdfs.at(id) -> NF(mu));
        double y = x/z;
        if(y >= 1) return 0;
        if(z >= 1) return 0;

        #ifdef DEBUG
        static bool first = true;
        if(first) {
            first = false;
            DLOG_F(INFO,"NOTE: IN OUTPUT, NO NF DEPEDENCE IS INCLUDED");
        }
        #endif

        double CxFQQ, CxFQG, CxFQQB, CxFQQP, CxFGG, CxFGQ;

        // Choose whether to convolute with PDF or with Splitting Function and PDF
        // This is done to only have to code up the convolution function once to reduce typos
        if(pid != 0) {
            if(mode == 0) {
                fpdf = pdfs.at(id) -> Apdf(pid, y, mu);
                fpdfg = pdfs.at(id) -> Apdf(0,y,mu);
                fpdfqb = pdfs.at(id) -> Apdf(-pid, y, mu);
                for(int i = 1; i <= nfEff; i++) {
                    if(std::abs(i) != std::abs(pid)) {
                        fpdfqp += pdfs.at(id) -> Apdf(i, y, mu);
                        fpdfqp += pdfs.at(id) -> Apdf(-i, y, mu);
                    }
                }
            } else {
                std::map<int, double> hoppetOut = hoppet -> GetConvolution(y, mu, 1);
                fpdf = hoppetOut[pid];
                fpdfg = hoppetOut[0];
                fpdfqb = hoppetOut[-pid];
                for(int i = 1; i <= nfEff; i++) {
                    if(abs(i) != abs(pid)) {
                        fpdfqp += hoppetOut[i];
                        fpdfqp += hoppetOut[-i];
                    }
                }
            }
            // Quark channel
            if(CQQMap.find(z) == CQQMap.end() ) { 
                CxFQQ = CA*CF*((1.0+z*z)/(1-z)*(-xLi(3,1-z)/2.0+xLi(3,z)-xLi(2,z)*log(z)/2.0
                            -0.5*xLi(2,z)*log(1-z)-1.0/24.0*pow(log(z),3)-0.5*pow(log(1-z),2)*log(z)
                            +1.0/12.0*pi2*log(1-z)-pi2/8)+1.0/(1.0-z)*(-0.25*(11-3*z*z)*ZETA3
                            -1.0/48.0*(-z*z+12*z+11)*pow(log(z),2)-1.0/36.0*(83.0*z*z-36.0*z+29.0)
                            *log(z)+pi2*z/4.0)
                        +(1-z)*(xLi(2,z)/2.0+0.5*log(1-z)*log(z))+(z+100)/27.0+0.25*z*log(1-z));
                CxFQQ += CF*CF*((1.0+z*z)/(1.0-z)*(xLi(3,1-z)/2.0+xLi(2,z)*log(1.0-z)/2.0
                            +3.0*xLi(2,z)*log(z)/2.0+3.0/4.0*log(z)*pow(log(1.0-z),2)
                            +pow(log(z),2)*log(1-z)/4.0
                            -1.0/12.0*pi2*log(1-z))
                        +(1-z)*(-xLi(2,z)-1.5*log(1-z)*log(z)+2.0*pi2/3.0-29.0/4.0)
                        +1.0/24.0*(1+z)*pow(log(z),3)
                        +1.0/(1.0-z)*(1.0/8.0*(-2*z*z+2*z+3)*pow(log(z),2)
                            +0.25*(17*z*z-13*z+4)*log(z))-z/4.0*log(1-z));
                CxFQQ += CF*CF*((1.0+z*z)/(1.0-z)*(5*(ZETA3-xLi(3,z))/2.0));
                CxFQQ -= CF*CF/4.0*((2*pi2-18)*(1-z)-(1+z)*log(z));
                CxFQQ += CF*(1/z*(1-z)*(2*z*z-z+2)*(xLi(2,z)/6.0+1.0/6.0*log(1-z)*log(z)-pi2/36)
                        +1.0/(216*z)*(1-z)*(136*z*z-143*z+172)-1.0/48.0*(8*z*z+3*z+3)*pow(log(z),2)
                        +1.0/36.0*(32*z*z-30*z+21)*log(z)+1.0/24.0*(1+z)*pow(log(z),3));
                CQQMap[z] = CxFQQ;
                DLOG_F(3,"CxFQQ(%e) = %e",z,CxFQQ);
            } else {
                CxFQQ = CQQMap[z];
            }
            CxFQQ += CF*nfEff*((1.0+z*z)/(72.0*(1.0-z))*log(z)*(3.0*log(z)+10.0)
                    + (-19.0*z-37.0)/108.0);
            CxFQQ /= 2.0;
            CxFQQ *= fpdf/z;

            // Gluon channel
            if(CQGMap.find(z) == CQGMap.end() ) { 
                CxFQG = CA*(-1.0/(12*z)*(1-z)*(11*z*z-z+2)*xLi(2,1-z)
                        +(2*z*z-2*z+1)*(xLi(3,1-z)/8-xLi(2,1-z)*log(1-z)/8
                            +pow(log(1-z),3)/48)
                        + (2*z*z+2*z+1)*(3*xLi(3,-z)/8+xLi(3,1/(1+z))/4.0
                            -xLi(2,-z)*log(z)/8-pow(log(1+z),3)/24+pow(log(z),2)*log(1+z)/16.0
                            +pi2/48*log(1+z))
                        +0.25*z*(1+z)*xLi(2,-z)+z*xLi(3,z)-0.5*z*xLi(2,1-z)*log(z)
                        -z*xLi(2,z)*log(z)-3.0/8.0*(2*z*z+1)*ZETA3-149*z*z/216.0
                        -(44*z*z-12*z+3)/96*pow(log(z),2)
                        +(68*z*z+6*pi2*z-30*z+21)/72.0*log(z)+pi2*z/24.0+43*z/48.0+43/(108*z)
                        +(2*z+1)/48.0*pow(log(z),3)-0.5*z*log(1-z)*pow(log(z),2)
                        -(1-z)/8.0*z*pow(log(1-z),2)
                        +0.25*z*(1+z)*log(1+z)*log(z)+(3-4*z)/16.0*z*log(1-z)-35.0/48.0);
                CxFQG += CF*((2*z*z-2*z+1)*(ZETA3-xLi(3,1-z)/8.0-xLi(3,z)/8.0
                            +xLi(2,1-z)*log(1-z)/8.0+xLi(2,z)*log(z)/8.0-pow(log(1-z),3)/48.0
                            +log(z)*pow(log(1-z),2)/16.0+pow(log(z),2)*log(1-z)/16.0)-3*z*z/8.0
                        -(4*z*z-2*z+1)/96.0*pow(log(z),3)+(-8*z*z+12*z+1)/64.0*pow(log(z),2)
                        +(-8*z*z+23*z+8)/32.0*log(z)+5*pi2/24.0*(1-z)*z+11*z/32.0
                        +(1-z)*z/8.0*pow(log(1-z),2)-0.25*(1-z)*z*log(1-z)*log(z)
                        -(3-4*z)*z/16.0*log(1-z)-9.0/32.0);
                CxFQG -= CF/4.0*(z*log(z)+0.5*(1-z*z)+(pi2-8)*z*(1-z));
                CQGMap[z] = CxFQG;
                DLOG_F(3,"CxFQG(%e) = %e",z,CxFQG);
            } else {
                CxFQG = CQGMap[z];
            }
            CxFQG *= fpdfg/z;

            // QBar channel
            if(CQQBMap.find(z) == CQQBMap.end() ) { 
                CxFQQB = CF*(CF-0.5*CA)*((1+z*z)/(1+z)*(3*xLi(3,-z)/2.0+xLi(3,z)+xLi(3,1.0/(1+z))
                            -xLi(2,-z)*log(z)/2.0-xLi(2,z)*log(z)/2.0-pow(log(z),3)/24.0
                            -pow(log(1+z),3)/6.0+0.25*log(1+z)*pow(log(z),2)+pi2/12.0*log(1+z)
                            -3*ZETA3/4.0)+(1-z)*(xLi(2,z)/2.0+0.5*log(1-z)*log(z)+15.0/8.0)
                        -0.5*(1+z)*(xLi(2,-z)+log(z)*log(1+z))+pi2/24.0*(z-3)+(11*z+3)/8.0*log(z));
                CxFQQB += CF*(1.0/(12*z)*(1-z)*(2*z*z-z+2)*(xLi(2,z)+log(1-z)*log(z)-pi2/6.0)
                        +1.0/(432*z)*(1-z)*(136*z*z-143*z+172)-(8*z*z+3*z+3)/96.0*pow(log(z),2)
                        +(32*z*z-30*z+21)/72.0*log(z)+(1+z)/48.0*pow(log(z),3));
                CQQBMap[z] = CxFQQB;
                DLOG_F(3,"CxFQQB(%e) = %e",z,CxFQQB);
            } else {
                CxFQQB = CQQBMap[z];
            }
            CxFQQB *= fpdfqb/z;

            // QPrime channel
            if(CQQPMap.find(z) == CQQPMap.end() ) { 
                CxFQQP = CF*((1-z)/(12*z)*(2*z*z-z+2)*(xLi(2,z)+log(1-z)*log(z)-pi2/6)
                        +(1-z)/(432*z)*(136*z*z-143*z+172)+(1+z)/48.0*pow(log(z),3)
                        -(8*z*z+3*z+3)/96.0*pow(log(z),2)+(32*z*z-30*z+21)/72.0*log(z));
                CQQPMap[z] = CxFQQP;
                DLOG_F(3,"CxFQQP(%e) = %e",z,CxFQQP);
            } else {
                CxFQQP = CQQPMap[z];
            }
            CxFQQP *= fpdfqp/z;

            return CxFQQ + CxFQG + CxFQQB + CxFQQP;
        } else {
            if(mode == 0) {
                fpdfg = pdfs.at(id) -> Apdf(pid, y, mu);
                for(int ipid = 1; ipid <= nfEff; ipid++) {
                    fpdf += pdfs.at(id) -> Apdf(ipid, y, mu);
                    fpdf += pdfs.at(id) -> Apdf(-ipid, y, mu);
                }
            } else {
                std::map<int, double> hoppetOut = hoppet -> GetConvolution(y, mu, 1);
                fpdfg = hoppetOut[pid];
                for(int ipid = 1; ipid <= nfEff; ipid++) {
                    fpdf += hoppetOut[ipid];
                    fpdf += hoppetOut[-ipid];
                }
            }

            // Gluon Channel
            if(CGGMap.find(z) == CGGMap.end() ) { 
                double zPlus = 1+z+z*z;
                double zMinus = 1-z+z*z;
                CxFGG = CA*CA*(pow(zPlus,2)/(z*(1+z))*(2*xLi(3,z/(1+z))-xLi(3,-z))
                        +(2-17*z-22*z*z-10*pow(z,3)-12*pow(z,4))/(2*z*(1+z))*ZETA3
                        -(5-z+5*z*z+pow(z,3)-5*pow(z,4)+pow(z,5))/(z*(1-z)*(1+z))*(xLi(3,z)-ZETA3)
                        +xLi(2,z)*log(z)/(1-z)*(3-z+3*z*z+pow(z,3)-3*pow(z,4)+pow(z,5))/(z*(1+z))
                        +pow(zPlus,2)/(z*(1+z))*(log(z)*xLi(2,-z)-1.0/3.0*pow(log(1+z),3)
                            +pi2/6*log(1+z))
                        +(1-z)/(3*z)*(11-z+11*z*z)*xLi(2,1-z)+1.0/12.0*z*log(1-z)
                        -1.0/6.0*pow(log(z),3)/(1-z)*pow(1+z-z*z,2)/(1+z)
                        +pow(log(z),2)*(pow(zMinus,2)/(2*z*(1-z))*log(1-z)
                            -pow(zPlus,2)/(2*z*(1+z))*log(1+z)+(25-11*z+44*z*z)/24.0)
                        +log(z)*(pow(zPlus,2)/(z*(1+z))*pow(log(1+z),2)
                            +pow(zMinus,2)/(2*z*(1-z))*pow(log(1-z),2)
                            -(72+773*z+149*z*z+536*z*z*z)/(72*z))
                        +517.0/27.0-449.0/(27.0*z)-380*z/27.0+835*z*z/54.0);
                CxFGG += CA*CA*((1+z)/z*log(z)+2*(1-z)/z);
                CGGMap[z] = CxFGG;
                DLOG_F(3,"CxFGG(%e) = %e",z,CxFGG);
            }  else {
                CxFGG = CGGMap[z];
            }
            CxFGG += CA*nfEff*((1+z)/12*pow(log(z),2)+1.0/36.0*(13+10*z)*log(z)-z/12.0*log(1-z)
                    -83.0/54.0+121/(108*z)+55.0/54.0*z-139/108.0*z*z)
                +CF*nfEff*((1+z)/12*pow(log(z),3)+1.0/8.0*(3+z)*pow(log(z),2)
                        +3.0/2.0*(1+z)*log(z)-(1-z)/(6*z)*(1-23*z+z*z));
            CxFGG /= 2.0;
            CxFGG *= fpdfg/z;

            // Quark Channel
            if(CGQMap.find(z) == CGQMap.end() ) { 
                CxFGQ = CF*CF*(1.0/48.0*(2-z)*pow(log(z),3)-1.0/32.0*(3*z+4)*pow(log(z),2)
                        +5.0/16.0*(z-3)*log(z)+1.0/12.0*(1/z+z/2-1)*pow(log(1-z),3)
                        +1.0/16.0*(z+6.0/z-6)*pow(log(1-z),2)
                        +(5.0*z/8.0+2.0/z-2)*log(1-z)+5.0/8.0-13.0/16.0*z)
                    +CF*CA*(-(1+pow(1+z,2))/(2*z)*xLi(3,1/(1+z))+(0.5-5/(2*z)-5.0/4.0*z)*xLi(3,z)
                            -3.0/(4*z)*(1+pow(1+z,2))*xLi(3,-z)+(2-11/(6*z)-z/2+z*z/3
                                +(-0.5+3/(2*z)+3*z/4)*log(z))*xLi(2,z)
                            +(z/4.0+(1+pow(1+z,2))/(4*z)*log(z))*xLi(2,-z)
                            +(1+pow(1+z,2))/(12*z)*pow(log(1+z),3)-1.0/(24*z)*((1+pow(1+z,2))
                                *(3*pow(log(z),2)+pi2)-6*z*z*log(z))*log(1+z)
                            -(1+pow(1-z,2))/(24*z)*pow(log(1-z),3)+1/(48*z)*(6*(1+pow(1-z,2))*log(z)
                                -5*z*z-22*(1-z))*pow(log(1-z),2)
                            +1.0/(72*z)*(-152+152*z-43*z*z+6*(-22+24*z-9*z*z+4*z*z*z)*log(z)
                                +9*(1+pow(1-z,2))*pow(log(z),2))*log(1-z)
                            -1.0/12.0*(1+z/2)*pow(log(z),3)+1.0/48.0*(36+9*z+8*z*z)*pow(log(z),2)
                            +(-107/24.0-1/z+z/12-11.0/9.0*z*z)*log(z)
                            +1.0/z*(4*ZETA3-503.0/54.0+11.0/36.0*pi2)+1007.0/108.0-pi2/3.0-5.0/2.0*ZETA3
                            +z*(pi2/3.0+2*ZETA3-133.0/108.0)+z*z*(38.0/27.0-pi2/18.0)); 
                CxFGQ += CF*CF*3.0/4.0*z+CF*CA/z*((1+z)*log(z)+2*(1-z)-(5+pi2)/4.0*z*z); 
                CGQMap[z] = CxFGQ;
                DLOG_F(3,"CxFGQ(%e) = %e",z,CxFGQ);
            } else {
                CxFGQ = CGQMap[z];
            }
            CxFGQ += CF*nfEff*(1.0/(24*z)*(1+pow(1-z,2))*pow(log(1-z),2)+1.0/18.0*(z+5/z-5)*log(1-z)
                    -14.0/27.0+14.0/(27*z)+13.0/108.0*z);
            CxFGQ *= fpdf/z;

            return CxFGG+CxFGQ;
        }
    }

    // Convolution of the Plus function with the PDF
    double Convolution::SxF(int pid,double x,double mu,double z,int mode, std::thread::id id) const {
        double fpdf1, fpdf2;
        // Choose whether to convolute with PDF or with Splitting Function and PDF
        // This is done to only have to code up the convolution function once to reduce typos
        if(mode == 0) {
            fpdf1 = pdfs.at(id) -> Apdf(pid, x/z, mu);
            fpdf2 = pdfs.at(id) -> Apdf(pid, x, mu);
        } else {
            std::map<int, double> hoppetOut = hoppet -> GetConvolution(x/z, mu, 1);
            fpdf1 = hoppetOut[pid];
            hoppetOut = hoppet -> GetConvolution(x, mu, 1);
            fpdf2 = hoppetOut[pid];
        }

        return (fpdf1/z-fpdf2)/(1.0-z);
    }

    // Convolution of G1 with the PDF
    double Convolution::G1xF(int pid, double x, double mu, double z, int mode, std::thread::id) const {
        double fpdf, fpdfg;
        // Choose whether to convolute with PDF or with Splitting Function and PDF
        // This is done to only have to code up the convolution function once to reduce typos
        fpdf = 0;
        if(mode == 0) {
            fpdfg = pdfs.at(mainThread) -> Apdf(0,z,mu);
            for(int ipid = -5; ipid < 6; ipid++) {
                if(ipid == 0) continue;
                fpdf += pdfs.at(mainThread) -> Apdf(ipid,z,mu);
            }
        } else {
            int j = mode/10;
            int i = mode%10;
            std::map<int, double> hoppetOut = hoppet -> GetConvolution(z, mu, i, j);
            fpdfg = hoppetOut[pid];
            for(int ipid = -5; ipid < 6; ipid++) {
                if(ipid == 0) continue;
                fpdf += hoppetOut[ipid];
            }
        }

        double y = x/z;

        return QCD::CF*(1-y)/y*fpdf/z+QCD::CA*(1-y)/y*fpdfg/z;
    }

    ConvGrid Convolution::GetGrid(const Conv &conv) const {
        switch(conv) {
            case Conv::C1:
                return C1f;
            case Conv::C1P1:
                return C1P1f;
            case Conv::C1P1P1:
                return C1P1P1f;
            case Conv::C1P2:
                return C1P2f;
            case Conv::C2:
                return C2f;
            case Conv::C2P1:
                return C2P1f;
            case Conv::G1:
                return G1f;
            case Conv::G1P1:
                return G1P1f;
            default:
                throw std::runtime_error("Convolution: Requested grid is not stored");
        }
    }

    bool Convolution::SaveGrid(const Conv &conv) const {
        // Get the data needed to save the grid
        std::map<int, Utility::Grid2D*> grid;
        std::string gridString;
        switch(conv) {
            case Conv::C1:
                grid = C1f;
                gridString = "C1";
                break;
            case Conv::C1P1:
                grid = C1P1f;
                gridString = "C1P1";
                break;
            case Conv::C1P1P1:
                grid = C1P1P1f;
                gridString = "C1P1P1";
                break;
            case Conv::C1P2:
                grid = C1P2f;
                gridString = "C1P2";
                break;
            case Conv::C2:
                grid = C2f;
                gridString = "C2";
                break;
            case Conv::C2P1:
                grid = C2P1f;
                gridString = "C2P1";
                break;
            case Conv::G1:
                grid = G1f;
                gridString = "G1";
                break;
            case Conv::G1P1:
                grid = G1P1f;
                gridString = "G1P1";
                break;
            default:
                throw std::runtime_error("Convolution: Requested grid is not to be saved");
        }
        
        // Get remaining needed metadata
        std::string pdfName = pdf -> GetName();
        const int iset = pdf -> GetISet();

        // Create unique filename
        std::string filename = pdfName + "_" + std::to_string(iset) + "_Conv_" + gridString + ".out";

#ifdef HAVE_GZIP
        // Open output file
        ogzstream gridOut(("ConvGrids/" + filename + ".gz").c_str());
#else
        // Open output file
        std::ofstream gridOut("ConvGrids/" + filename);
#endif
        // Check to ensure the file opens correctly
        if(!gridOut.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        // Write metadata
        gridOut << std::setprecision(8) << std::scientific << std::left;
        gridOut << std::setw(20) << "PDF Name: " << pdfName << '\n';
        gridOut << std::setw(20) << "Member Set: " << iset << '\n';
        gridOut << std::setw(20) << "Convolution Order: " << gridString << '\n';
        gridOut << "------------------" << '\n';
        for(unsigned int i = 0; i < grid[0] -> GetXDim(); ++i)
            gridOut << std::setw(16) << grid[0] -> GetX(i);
        gridOut << '\n';
        for(unsigned int i = 0; i < grid[0] -> GetYDim(); ++i)
            gridOut << std::setw(16) << grid[0] -> GetY(i);
        gridOut << '\n';
        gridOut << "------------------" << '\n';
        gridOut << std::setw(16) << "-5" << std::setw(16) << "-4" << std::setw(16) << "-3";
        gridOut << std::setw(16) << "-2" << std::setw(16) << "-1" << std::setw(16) << " 0";
        gridOut << std::setw(16) << " 1" << std::setw(16) << " 2" << std::setw(16) << " 3";
        gridOut << std::setw(16) << " 4" << std::setw(16) << " 5" << '\n';

        // Write out values for the grid
        for(size_t i = 0; i < grid[0] -> GetXDim(); ++i) {
            for(size_t j = 0; j < grid[0] -> GetYDim(); ++j) {
                for(int k = -5; k < 6; ++k) {
                    gridOut << std::setw(16) << grid[k] -> FindPoint(i, j);
                }
                gridOut << '\n';
            }
        }

        return true;
    }

    bool Convolution::LoadGrid(const Conv &conv) {
        // Setup needed metadata
        std::string pdfName = pdf -> GetName();
        int iset = pdf -> GetISet();
        std::string gridString;
        switch(conv) {
            case Conv::C1:
                gridString = "C1";
                break;
            case Conv::C1P1:
                gridString = "C1P1";
                break;
            case Conv::C1P1P1:
                gridString = "C1P1P1";
                break;
            case Conv::C1P2:
                gridString = "C1P2";
                break;
            case Conv::C2:
                gridString = "C2";
                break;
            case Conv::C2P1:
                gridString = "C2P1";
                break;
            case Conv::G1:
                gridString = "G1";
                break;
            case Conv::G1P1:
                gridString = "G1P1";
                break;
            default:
                throw std::runtime_error("Convolution: Requested grid is not possible to load");
        }

        // Check if file exists
        std::string filename = pdfName + "_" + std::to_string(iset) + "_Conv_" + gridString + ".out";

        // Initialize temp map to hold values
        std::map<int, Utility::Grid2D*> grid; 
        for(int i = -5; i < 6; ++i)
            grid[i] = new Utility::Grid2D(xVec, qVec);

#ifdef HAVE_GZIP
        try {
            // Load file
            igzstream gridIn(("ConvGrids/" + filename + ".gz").c_str());

            // Check to see if the file exists zipped, if not try with an unzipped version
            if(!gridIn.is_open())
                throw std::runtime_error("No gzip file");

            // Skip over the header since it is not needed
            std::string line;
            for(unsigned int i = 0; i < 8; ++i) 
                std::getline(gridIn, line);
    
            // Read lines in
            double value;
            for(size_t i = 0; i < grid[0] -> GetXDim(); ++i) {
                for(size_t j = 0; j < grid[0] -> GetYDim(); ++j) {
                    for(int pid = -5; pid < 6; ++pid) {
                        gridIn >> value;
                        grid[pid] -> AddPoint(i, j, value);
                    }
                }
            }

        // Catch a non-exsistent gzip file and search to see if there is one unzipped
        } catch (std::exception &e) {
            std::ifstream gridIn("ConvGrids/" + filename);

            if(!gridIn.is_open()) {
                LOG_F(INFO,"Could not load file: %s",filename.c_str());
                return false;
            }

            // Skip over the header since it is not needed
            std::string line;
            for(size_t i = 0; i < 8; ++i) 
                std::getline(gridIn, line);
    
            // Read lines in
            double value;
            for(size_t i = 0; i < grid[0] -> GetXDim(); ++i) {
                for(size_t j = 0; j < grid[0] -> GetYDim(); ++j) {
                    for(int pid = -5; pid < 6; ++pid) {
                        gridIn >> value;
                        grid[pid] -> AddPoint(i, j, value);
                    }
                }
            }
        }
#else //NOT HAVE_GZIP
        // Load file
        std::ifstream gridIn("ConvGrids/" + filename);

        if(!gridIn.is_open()) {
            LOG_F(INFO,"Could not load file: %s", filename.c_str());
            return false;
        }

        // Skip over the header since it is not needed
        std::string line;
        for(size_t i = 0; i < 8; ++i) 
            std::getline(gridIn, line);

        // Read lines in
        double value;
        for(size_t i = 0; i < grid[0] -> GetXDim(); ++i) {
            for(size_t j = 0; j < grid[0] -> GetYDim(); ++j) {
                for(int pid = -5; pid < 6; ++pid) {
                    gridIn >> value;
                    grid[pid] -> AddPoint(i, j, value);
                }
            }
        }
#endif //HAVE_GZIP

        switch(conv) {
            case Conv::C1:
                C1f = grid;
                break;
            case Conv::C1P1:
                C1P1f = grid;
                break;
            case Conv::C1P1P1:
                C1P1P1f = grid;
                break;
            case Conv::C1P2:
                C1P2f = grid;
                break;
            case Conv::C2:
                C2f = grid;
                break;
            case Conv::C2P1:
                C2P1f = grid;
                break;
            case Conv::G1:
                G1f = grid;
                break;
            case Conv::G1P1:
                G1P1f = grid;
                break;
            default:
                throw std::runtime_error("Convolution: Requested grid is not possible to load");
        }

        return true;
    }
}

