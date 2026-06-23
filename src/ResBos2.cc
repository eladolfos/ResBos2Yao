#include <cmath>

#include "ResBos/ResBos2.hh"
#include "ResBos/Process.hh"
#include "ResBos/PhaseSpace.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/GridIO.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/FourVector.hh"

#include "ResBos/PeakFinder.hh"

namespace ResBos {
    using namespace QCD;
    
    ResBos::ResBos() {}

    ResBos::ResBos(const IO::Settings &s, Process *p) : process(p) {

    }

    void ResBos::Setup(Process *p, const IO::Settings &s) {
        process = p;
        std::string schemeStr = s.GetSetting<std::string>("Scheme");
        if(schemeStr.compare("CSS") == 0) scheme = Scheme::CSS;
        else if(schemeStr.compare("CFG") == 0) scheme = Scheme::CFG;
        else throw std::runtime_error("Process: Invalid scheme choice. Valid options are CSS or CFG");

        // Initialize A and B coefficients
        A2.resize(3); A3.resize(3); B1.resize(3); B2.resize(3); B3.resize(3);
        if(process->GetInitState() == InitialState::qq) A1 = CF;
        else if(process->GetInitState() == InitialState::gg) A1 = CA;
        else throw std::runtime_error("Process: Currently only qq and gg initial states are supported");
        // Canonical Values
        for(int i = 0; i < 3; i++) {
            int nf = i+3;
            double K = (67.0/18.0 - M_PI*M_PI/6.0)*CA-5.0/9.0*nf;
            double gamma;
            if(process->GetInitState() == InitialState::qq) {
                gamma = (-3+24*M_PI*M_PI/6.0-48*ZETA3)*CF*CF+(-17.0/3.0-88.0/3.0*M_PI*M_PI/6.0+24*ZETA3)*CF*CA+(2.0/3.0+16.0/3.0*M_PI*M_PI/6.0)*CF*nf;
                B1[i] = CF*(-3.0/2.0);
                B3[i] = (7358.86-721.516*nf+20.5951*nf*nf)/pow(4.0,3)-(beta1(nf)*process->H1()+2*beta0(nf)*(process->H2(nf)-0.5*pow(process->H1(),2)));
            } else if(process->GetInitState() == InitialState::gg) {
                gamma = (-64.0/3.0-24*ZETA3)*CA*CA+16.0/3.0*CA*nf+4*CF*nf;
                B1[i] = -2*beta0(nf);
                B3[i] = (35134.6-7311.10*nf+293.017*nf*nf-(836+184*nf-14.2222*nf*nf)
                        *log(pow(process->GetEW()->GetMtReal()/process->GetEW()->GetMhReal(),2)))/pow(4.0,3)
                        -(beta1(nf)*process->H1()+2*beta0(nf)*(process->H2(nf)-0.5*pow(process->H1(),2)));
            } else {
                throw std::runtime_error("Process: Currently only qq and gg initial states are supported");
            }
            A2[i] = 0.5*K*A1;
            A3[i] = A1*(CF*nf*0.5*(ZETA3-55.0/48.0)-nf*nf/108.0+CA*CA*(11*ZETA3/24.0+11*pow(M_PI,4)/720.0-67.0*M_PI*M_PI/216.0+245.0/96.0)
                    + CA*nf*(-7.0*ZETA3/12.0+5.0*M_PI*M_PI/108.0-209.0/432.0));
            B2[i] = gamma/16.0+beta0(nf)*A1*M_PI*M_PI/6.0;

            // Adjust B2 and B3 based on the scheme choice
            if(scheme==Scheme::CSS) {
                B2[i] += beta0(nf)*process->H1();
                B3[i] += beta1(nf)*process->H1()+2*beta0(nf)*(process->H2(process->GetEW()->GetMhReal())-0.5*pow(process->H1(),2));
            }
        }
    }

    void ResBos::GridSetup(Process *p, const IO::Settings &s, std::map<std::thread::id,Utility::PDF*> pdfs, Utility::Hoppet* hoppet) {
        //Check if the grid has already been generated to save computation time
        IO::GridIO gridIO(s);
        if(gridIO.FindGrid3D(gType)) {
            grid=gridIO.LoadGrid3D();
        } else {
            // Calculate the needed convolution grids if needed
            Convolution *convolution = new Convolution(s.GetSetting<int>("COrder"),GetScheme(),this,s);
            std::vector<Conv> convs = GetNeededConvs();

            //Check for each grid before generating it. If it exists load it instead
            for(auto conv : convs) {
                if(gridIO.FindGrid2D(conv)) {
                    convolution->SetHoppet(hoppet);
                    convolution->SetPDFs(pdfs);
                    convolution->SetGrid(conv,gridIO.LoadGrid2D());
                } else {
                    convolution->GenerateGrid(process,pdfs,hoppet,conv);
                    gridIO.SaveGrid2D(convolution->GetGrid(conv));
                }
            }
            if(gType==GridType::Resum || gType==GridType::Tot)
//                convolution->GenerateGrid(process,pdfs,hoppet,Conv::C);
//            if(settings.KeyExisits("ScaleVar") || settings.KeyExisits("PDFVar")) process -> SetVariations(GetVariations(settings,process,hoppet,convolution));
//            process -> InitHist();

            // Find the peak location for the resummed piece
            //qMin = s.GetSetting<double>("Qmin");
            //qMax = s.GetSetting<double>("Qmax");
            //PeakFinder p(this);
            //p.FindPeaks(qMin,qMax);
            //exit(-1);

            // Generate Q, qt, y grid if not fitting
            if(s.GetSetting<std::string>("Mode") != "Fit") {
                qMin = s.GetSetting<double>("Qmin");
                qMax = s.GetSetting<double>("Qmax");
                qTMin = s.GetSetting<double>("qTmin");
                qTMax = s.GetSetting<double>("qTmax");
                yMin = s.GetSetting<double>("Ymin");
                yMax = s.GetSetting<double>("Ymax");
                GridGen();

                gridIO.SaveGrid3D(grid);
            }

            beams.first.SetConvolution(convolution);
            beams.second.SetConvolution(convolution);

        }

    }

    void ResBos::GridGen() {
        int nQ = (qMax-qMin)/5+1;
        if(qTMin == 0) qTMin += 1E-1;
        int nQT = 10*log(qTMax/qTMin)+3;
        int nY = (yMax-yMin)/0.1+1;

        for(int i = 0; i < nQ; i++) qVals.push_back(qMin+i*5);
        for(int i = 0; i < nQT; i++) qTVals.push_back(qTMax*exp(-0.1*(nQT-i-1)));
        for(int i = 0; i < nY; i++) yVals.push_back(yMin+i*0.1);

        std::vector<double> test = GetCalc(qVals[0],qTVals[0],yVals[0]);
        for(int i = 0; i < test.size(); i++) {
            Utility::Grid3D *g = new Utility::Grid3D(qVals,yVals,qTVals);
            grid[i] = g;
        }

        std::cout << "Generating the grid for the calculation: " << std::endl;
        std::vector<std::shared_future<void>> futures;
        for(auto q : qVals) {
            for(auto y : yVals) {
                futures.emplace_back(
                    pool->enqueue([this,q,y] {
                        for(auto qT : qTVals) {
                                std::vector<double> result = GetCalc(q,qT,y);
                                for(int i = 0; i < result.size(); i++) {
                                    grid[i] -> AddPoint(q,y,qT,result[i]);
                                }
                        }
                }));
            }
        }
        WaitAll(futures);
    }

    // Returns maximum rapidity for qt=0
    double ResBos::GetYMax(double Q) {
        return acosh(process->ECM()/(2*Q)); 
    }

    // Returns maximum rapidity for given qt
    double ResBos::GetYMax(double Q, double qt) {
        double t1 = pow(process->ECM(),2);
        double t2 = sqrt(qt*qt+Q*Q);
        double t3 = 2*process->ECM()*t2;

        return acosh(t1/t3);
    }

    double ResBos::H(const int order, const double Q) { 
        if(order == 0) return 1;
        else if(order == 1) return 1+process->Alpi(Q)*process->H1();
        else if(order == 2) return 1+process->Alpi(Q)*(process->H1()+process->Alpi(Q)*process->H2(Q));
    }

    std::function<double(double*,double)> ResBos::GetXSect() {
        auto func = [this](double *x, double wgt) {return XSect(x,wgt); };
        return func;
    }

    std::mutex testing;
    double ResBos::XSect(double *x, double wgt) {
        std::vector<double> Point(process -> GetNDim());
        std::vector<FourVector> momenta;
        double PSwt = process->GetPS()->Wresph(x,2.5,Point,momenta);
        double Q = Point[0], qt = Point[1], y = Point[2];
        if(2.4 < std::abs(y)) return 0;
        if(momenta[1].Pt() < 20 || momenta[2].Pt() < 20) return 0;
        if(fabs(momenta[1].Rapidity()) > 2.4 || fabs(momenta[2].Rapidity()) > 2.4) return 0;
        if(PSwt == 0) return 0;
        std::vector<double> result = GetPoint(Q,y,qt);
        double MEwt = process -> ME(Point,result);
        //        if(process -> WriteHist()) {
        //            double theta = acos(tanh((momenta[1].Eta()-momenta[2].Eta())/2.0));
        //            double phiStar = tan((M_PI-momenta[1].DeltaPhi(momenta[2]))/2.0)*sin(theta);
        //            process -> FillHist(0,qt,MEwt*PSwt*wgt*wgtB);
        //            process -> FillHist(2,b*qt,MEwt*PSwt*wgt*wgtB/Utility::BesJ0(b*qt));
        //            process -> FillHist(3,phiStar,MEwt*PSwt*wgt*wgtB);
        //            for(int i = 0; i < process -> GetVariations().size(); i++) {
        //                std::vector<double> resumVar = GetResummationVegasVar(Q,qt,y,b,process -> GetVariation(i));
        //                double MEwtVar = process -> ME(MEPoint, resumVar);
        //                process -> FillHist(i+4,qt,MEwtVar*PSwt*wgt*wgtB);
        //            }
        //        }
        
        return MEwt*PSwt;
    }

    std::vector<double> ResBos::GetPoint(double Q, double qt, double y) {
        std::vector<double> result;
        for(int i = 0; i < grid.size(); ++i) result.push_back(grid[i] -> Interpolate(Q,qt,y));
        return result;
    }

    std::pair<BDFMap,BDFMap> ResBos::CalcMap(int pid1, int pid2, double x1, double x2, double Q) const {
        int order = 0;
        if((gType==GridType::DelSig && GetDeltaSigmaOrder() > 0) || (gType==GridType::Asym && GetAsymOrder() > 1)) order++;
        if((gType==GridType::DelSig && GetDeltaSigmaOrder() > 1) || (gType==GridType::Asym && GetAsymOrder() > 2)) order++;

        BDFMap map1 = beams.first.BeamMap(order, pid1, x1, Q);
        BDFMap map2 = beams.first.BeamMap(order, pid2, x2, Q);

        if(scheme == Scheme::CFG) {
            int nf = process -> NF(Q);

            // Needed at alpha_s^2 for Asymptotic and alpha_s for DeltaSigma
            if(order > 0) {
                map1[BDF::C1] += 0.5*process->H1()*map1[BDF::PDF];
                map2[BDF::C1] += 0.5*process->H1()*map2[BDF::PDF];
                map1[BDF::C1P1] += 0.5*process->H1()*map1[BDF::P1];
                map2[BDF::C1P1] += 0.5*process->H1()*map2[BDF::P1];

                // Needed at alpha_s^3 for Asymptotic and alpha_s^2 for DeltaSigma
                if(order > 1) {
                    map1[BDF::C1P2] += 0.5*process->H1()*map1[BDF::P2];
                    map2[BDF::C1P2] += 0.5*process->H1()*map2[BDF::P2];
                    map1[BDF::C1P1P1] += 0.5*process->H1()*map1[BDF::P1P1];
                    map2[BDF::C1P1P1] += 0.5*process->H1()*map2[BDF::P1P1];
                    map1[BDF::C2] += 0.5*process->H1()*map1[BDF::C1]+0.5*(process->H2(nf)-0.5*pow(process->H1(),2))*map1[BDF::PDF];
                    map2[BDF::C2] += 0.5*process->H1()*map2[BDF::C1]+0.5*(process->H2(nf)-0.5*pow(process->H1(),2))*map2[BDF::PDF];
                    map1[BDF::C2P1] += 0.5*process->H1()*map1[BDF::C1P1]+0.5*(process->H2(nf)-0.5*pow(process->H1(),2))*map1[BDF::P1];
                    map2[BDF::C2P1] += 0.5*process->H1()*map2[BDF::C1P1]+0.5*(process->H2(nf)-0.5*pow(process->H1(),2))*map2[BDF::P1];
                }
            }
        }

        return std::make_pair(map1,map2);
    }

    double ResBos::GetConvolution(int hadron, int pid, double x, double Q, Conv c) const { 
        if(hadron == 0) return beams.first.PDF(Conv2BDF.at(c),pid,x,Q);
        return beams.second.PDF(Conv2BDF.at(c),pid,x,Q);
    }


}
