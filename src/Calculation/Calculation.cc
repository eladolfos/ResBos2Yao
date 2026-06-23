#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "ResBos/Calculation.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/Process.hh"
#include "ResBos/ProgressBar.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/ThreadPool.hh"

#include "ResBos/PhaseSpace.hh"

#include "ResBos/loguru.hpp"

#ifdef HAVE_GZIP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "gzstream/gzstream.h"
#pragma GCC diagnostic pop
#endif

#if USING_MPI
#include "ResBos/MPI.hh"

struct Point {
    double Q, qT, y;
};
#endif

//#include "ResBos/PeakFinder.hh"

namespace ResBos {
    using namespace QCD;

    Calculation::~Calculation() {
        for(auto g : grid) {
            delete g.second;
        }
    }

    void Calculation::Initialize(IO::Settings *s, std::shared_ptr<ResBos> resbos_) {
        resbos = resbos_;

        // Check if Kinematic Correction should be used
        if(s->GetSettingBool("KinematicCorrection")) {
            KinCorr = true;
        } else {
            KinCorr = false;
        }

        // Initialize the scheme
        std::string schemeStr = s->GetSettingString("Scheme");
        if(schemeStr.compare("CSS") == 0) scheme = Scheme::CSS;
        else if(schemeStr.compare("CFG") == 0) scheme = Scheme::CFG;
        else throw std::runtime_error("Process: Invalid scheme choice. Valid options are CSS or CFG");

        //if(AOrder == 5) {
        //    std::string A5ErrStr = s -> GetSettingString("A5Error");
        //    if(A5ErrStr.compare("Central") == 0) A5Err = A5Choice::Central;
        //    else if(A5ErrStr.compare("Plus") == 0) A5Err = A5Choice::Plus;
        //    else if(A5ErrStr.compare("Minus") == 0) A5Err = A5Choice::Minus;
        //}

        // Initialize A and B coefficients
        m_A2 = {}; m_A3 = {}; m_A4 = {}; m_A5 = {};
        m_B1 = {}; m_B2 = {}; m_B3 = {};
        InitialState initState = resbos -> GetProcess() -> GetInitState();
        if(initState == InitialState::qq) m_A1 = CF;
        else if(initState == InitialState::gg) m_A1 = CA;
        else throw std::runtime_error("Process: Currently only qq and gg initial states are supported");

        // Canonical Values
        double H1 = resbos -> GetProcess() -> H1();
        for(size_t i = 0; i < 3; i++) {
            double nf = static_cast<double>(i+3);
            double K = (67.0/18.0 - M_PI*M_PI/6.0)*CA-5.0/9.0*nf;
            double gamma;
            if(initState == InitialState::qq) {
                gamma = (-3+24*M_PI*M_PI/6.0-48*ZETA3)*CF*CF
                    +(-17.0/3.0-88.0/3.0*M_PI*M_PI/6.0+24*ZETA3)*CF*CA
                    +(2.0/3.0+16.0/3.0*M_PI*M_PI/6.0)*CF*nf;
                m_B1[i] = CF*(-3.0/2.0);
                m_B3[i] = (7358.86-721.516*nf+20.5951*nf*nf)/pow(4.0,3)
                    -(beta1(static_cast<size_t>(nf))*H1
                    +2*beta0(static_cast<size_t>(nf))
                    *(resbos -> GetProcess() -> H2(static_cast<size_t>(nf))
                    -0.5*pow(H1,2)));
                // From 1808.08981: A4,q = 20702(2) - 5171.9(2) nf + 195.5722 nf^2 + 3.272344 nf^3 (expansion is alphas/(4pi))
                m_A4[i] = (20702 - 5171.9*nf + 195.5722*nf*nf + 3.272344*nf*nf*nf)/pow(4.,4);

                // From 1812.11818: A5,q = (1.7 +/- 0.5, 1.1 +/- 0.5, 0.7 +/- 0.5) * 10^5 (expansion is alphas/(4pi))
                //if(AOrder == 5) {
                //    if(A5Err == A5Choice::Central) {
                //        if(nf == 3) A5[i] = 1.7e5/pow(4.0,4);
                //        else if(nf == 4) A5[i] = 1.1e5/pow(4.0,4);
                //        else if(nf == 5) A5[i] = 0.7e5/pow(4.0,4);
                //    } else if(A5Err == A5Choice::Plus) {
                //        if(nf == 3) A5[i] = 2.2e5/pow(4.0,4);
                //        else if(nf == 4) A5[i] = 1.6e5/pow(4.0,4);
                //        else if(nf == 5) A5[i] = 1.2e5/pow(4.0,4);
                //    } else if(A5Err == A5Choice::Minus) {
                //        if(nf == 3) A5[i] = 1.2e5/pow(4.0,4);
                //        else if(nf == 4) A5[i] = 0.6e5/pow(4.0,4);
                //        else if(nf == 5) A5[i] = 0.2e5/pow(4.0,4);
                //    }
                //}
            } else if(initState == InitialState::gg) {
                gamma = (-64.0/3.0-24*ZETA3)*CA*CA+16.0/3.0*CA*nf+4*CF*nf;
                m_B1[i] = -2*beta0(static_cast<size_t>(nf));
                m_B3[i] = (35134.6-7311.10*nf+293.017*nf*nf-(836+184*nf-14.2222*nf*nf)
                        *log(pow(resbos->GetProcess()->GetEW()->GetMtReal()
                                /resbos->GetProcess()->GetEW()->GetMhReal(),2)))/pow(4.0,3)
                        -(beta1(static_cast<size_t>(nf))*H1
                            +2*beta0(static_cast<size_t>(nf))
                            *(resbos -> GetProcess() -> H2(static_cast<size_t>(nf))
                            -0.5*pow(H1,2)));
                // From 1808.08981: A4,g = 40880(30) - 11714(2) nf + 440.0488 nf^2 + 7.362774 nf^3 (expansion is alphas/(4pi))
                m_A4[i] = (40880 - 11714*nf + 440.0488*nf*nf + 7.362774*nf*nf*nf)/pow(4.0,4);
            } else {
                throw std::runtime_error("Process: Currently only qq and gg initial states are supported");
            }
            m_A2[i] = 0.5*K*m_A1;
            m_A3[i] = m_A1*(CF*nf*0.5*(ZETA3-55.0/48.0)-nf*nf/108.0
                    +CA*CA*(11*ZETA3/24.0+11*pow(M_PI,4)/720.0-67.0*M_PI*M_PI/216.0+245.0/96.0)
                    + CA*nf*(-7.0*ZETA3/12.0+5.0*M_PI*M_PI/108.0-209.0/432.0));
            m_B2[i] = gamma/16.0+beta0(static_cast<size_t>(nf))*m_A1*M_PI*M_PI/6.0;

            // Adjust B2 and B3 based on the scheme choice
            if(scheme==Scheme::CSS) {
                m_B2[i] += beta0(static_cast<size_t>(nf))*H1;
                m_B3[i] += beta1(static_cast<size_t>(nf))*H1
                        +2*beta0(static_cast<size_t>(nf))
                        *(resbos -> GetProcess() -> H2(static_cast<size_t>(nf))-0.5*pow(H1,2));
            }
        }
        LOG_S(INFO) << "Scheme: " << schemeStr;
        LOG_F(INFO,"A[1] = %f", m_A1);
        LOG_F(INFO,"A[2](nf = 3,4,5) = %f, %f, %f",m_A2[0],m_A2[1],m_A2[2]);
        LOG_F(INFO,"A[3](nf = 3,4,5) = %f, %f, %f",m_A3[0],m_A3[1],m_A3[2]);
        LOG_F(INFO,"A[4](nf = 3,4,5) = %f, %f, %f",m_A4[0],m_A4[1],m_A4[2]);
        LOG_F(INFO,"A[5](nf = 3,4,5) = %f, %f, %f",m_A5[0],m_A5[1],m_A5[2]);
        LOG_F(INFO,"B[1](nf = 3,4,5) = %f, %f, %f",m_B1[0],m_B1[1],m_B1[2]);
        LOG_F(INFO,"B[2](nf = 3,4,5) = %f, %f, %f",m_B2[0],m_B2[1],m_B2[2]);
        LOG_F(INFO,"B[3](nf = 3,4,5) = %f, %f, %f",m_B3[0],m_B3[1],m_B3[2]);
    }

    void Calculation::SetScheme(Scheme s) {
        if(scheme == s) return;
        scheme = s;
        for(size_t i = 0; i < 3; ++i) {
            double nf = static_cast<double>(i+3);
            double H1 = resbos -> GetProcess() -> H1();
            if(scheme == Scheme::CSS) {
                m_B2[i] += beta0(static_cast<size_t>(nf))*H1;
                m_B3[i] += beta1(static_cast<size_t>(nf))*H1
                        +2*beta0(static_cast<size_t>(nf))
                        *(resbos -> GetProcess() -> H2(static_cast<size_t>(nf))-0.5*pow(H1,2));
            } else {
                m_B2[i] -= beta0(static_cast<size_t>(nf))*H1;
                m_B3[i] -= beta1(static_cast<size_t>(nf))*H1
                        +2*beta0(static_cast<size_t>(nf))
                        *(resbos -> GetProcess() -> H2(static_cast<size_t>(nf))-0.5*pow(H1,2));
            }
        }
        LOG_F(INFO, "Updating scheme parameters");
        LOG_F(INFO,"A[1] = %f", m_A1);
        LOG_F(INFO,"A[2](nf = 3,4,5) = %f, %f, %f",m_A2[0],m_A2[1],m_A2[2]);
        LOG_F(INFO,"A[3](nf = 3,4,5) = %f, %f, %f",m_A3[0],m_A3[1],m_A3[2]);
        LOG_F(INFO,"A[4](nf = 3,4,5) = %f, %f, %f",m_A4[0],m_A4[1],m_A4[2]);
        LOG_F(INFO,"A[5](nf = 3,4,5) = %f, %f, %f",m_A5[0],m_A5[1],m_A5[2]);
        LOG_F(INFO,"B[1](nf = 3,4,5) = %f, %f, %f",m_B1[0],m_B1[1],m_B1[2]);
        LOG_F(INFO,"B[2](nf = 3,4,5) = %f, %f, %f",m_B2[0],m_B2[1],m_B2[2]);
        LOG_F(INFO,"B[3](nf = 3,4,5) = %f, %f, %f",m_B3[0],m_B3[1],m_B3[2]);
    }

    void Calculation::GridSetup(const IO::Settings &s) {
            qMin = s.GetSetting<double>("Qmin");
            qMax = s.GetSetting<double>("Qmax");
            qTMin = s.GetSetting<double>("qTmin");
            qTMax = s.GetSetting<double>("qTmax");
            yMin = s.GetSetting<double>("Ymin");
            yMax = s.GetSetting<double>("Ymax");
    }

    void Calculation::GridGen() {
        if(qTMin == 0) qTMin += 1E-1;
        size_t nQT = static_cast<size_t>(10*log(qTMax/qTMin)+3);
        size_t nY = static_cast<size_t>((yMax-yMin)/0.1+1);

        double QVal = qMin;
        while(QVal <= qMax) { 
            qVals.push_back(QVal);
            if(QVal < 20) QVal += 1;
            else if(QVal < 200) QVal += 10;
            else if(QVal < 500) QVal += 50;
            else if(QVal < 1000) QVal += 100;
            else QVal += 500;
        } 
        for(size_t i = 0; i < nQT; i++) 
            qTVals.push_back(qTMax*exp(-0.1*(static_cast<double>(nQT-i)-1)));
        for(size_t i = 0; i < nY; i++) 
            yVals.push_back(yMin+static_cast<double>(i)*0.1);


        std::vector<double> test = GetCalc(qVals[0],qTVals[0],yVals[0]);
        for(size_t i = 0; i < test.size(); i++) {
            grid[i] = new Utility::Grid3D(qVals,yVals,qTVals);
        }

        std::cout << "Generating the grid for the calculation: " << std::endl;
        LOG_F(INFO,"Begin generating calculation grid");

#if USING_MPI
        // Define point type to ease distribution
        MPI_Datatype point3d;
        MPI_Type_contiguous(3,MPI_DOUBLE,&point3d);
        MPI_Type_commit(&point3d);

        // Create the array of structs to distribute
        std::vector<Point> points;
        for(size_t i = 0; i < qVals.size(); ++i) {
            for(size_t j = 0; j < yVals.size(); ++j) {
                for(size_t k = 0; k < qTVals.size(); ++k) {
                    if(std::abs(yVals[j]) > GetYMax(qVals[i],qTVals[k])) {
                        for(size_t l = 0; l < test.size(); ++l) {
                            grid[l] -> AddPoint(qVals[i],qTVals[k],yVals[j],0);
                        }
                        continue;
                    }
                    Point tmp;
                    tmp.Q = qVals[i];
                    tmp.qT = qTVals[k];
                    tmp.y = yVals[j];
                    points.push_back(tmp);
                }
            }
        }

        size_t count = points.size();
        const int size = mpi -> Size();
        const int rank = mpi -> Rank();
        int locCounts[size];
        int locDisplace[size];
        int rem = count % size;
        int sum = 0;

        for(int i = 0; i < size; ++i) {
            locCounts[i] = count / size;
            if(rem > 0) {
                locCounts[i]++;
                rem--;
            }

            locDisplace[i] = sum;
            sum += locCounts[i];
        }


        // Split up the grid across all the nodes
        std::vector<Point> localPts(count/size + 1);
        mpi -> Scatterv(&(points[0]), locCounts, locDisplace, point3d, 
                                &(localPts[0]), count/size+1, point3d);

        // Generate the grid points
        size_t resultSize = test.size();
        std::vector<double> result(resultSize*locCounts[rank]); 
        for(int i = 0; i < locCounts[rank]; ++i) {
            std::vector<double> tmp = GetCalc(localPts[i].Q,localPts[i].qT,localPts[i].y);
            for(size_t j = 0; j < resultSize; ++j) {
                result[i*resultSize+j] = tmp[j];
            }
        }

        // Collect the grid distributed across all the nodes
        sum = 0;
        for(int i = 0; i < size; ++i) {
            locCounts[i] = locCounts[i]*resultSize;
            locDisplace[i] = sum;
            sum += locCounts[i];
        }
        std::vector<double> resultAll(resultSize*count);
        std::cout << "Rank[" << rank << "] has finished" << std::endl;
        mpi -> GatherAllV(&(result[0]), result.size(), MPI_DOUBLE,
                          &(resultAll[0]), locCounts, locDisplace, MPI_DOUBLE);   

        // Fill in the grid
        for(size_t i = 0; i < count; ++i) {
            for(size_t j = 0; j < resultSize; ++j)  {
                grid[j] -> AddPoint(points[i].Q,points[i].y,points[i].qT,
                                    resultAll[i*resultSize+j]);
            }
        }
#else
        ProgressBar pb(qVals.size()*qTVals.size()*yVals.size());
        for(auto q : qVals) {
            for(auto y : yVals) {
                LOG_F(INFO, "Calculating grid for Q = %.2f; y = %.2f", q, y);
                for(auto qT : qTVals) {
                    pb.Update(1);
                    pb.Display();
                    std::vector<double> result = GetCalc(q,qT,y);
                    for(size_t i = 0; i < result.size(); i++) {
                        grid[i] -> AddPoint(q,y,qT,result[i]);
                    }
                }
            }
        }
        pb.Done();
        LOG_F(INFO,"Finished generating calculation grid");
#endif

    }

    // Returns maximum rapidity for qt=0
    double Calculation::GetYMax(const double &Q) const {
        return log(resbos->GetECM()/Q); 
    }

    // Returns maximum rapidity for given qt
    double Calculation::GetYMax(const double &Q, const double &qt) const {
        double t1 = pow(resbos->GetECM(),2)+Q*Q;
        double t2 = sqrt(qt*qt+Q*Q);
        double t3 = 2*resbos->GetECM()*t2;

        if(t1/t3 < 1) return -1;

        return acosh(t1/t3);
    }

    // Return maximum qt for a given y and Q
    double Calculation::GetQTMax(const double &Q, const double &y) const {
        if(std::abs(y) >= GetYMax(Q)) return 0;
        double ecm = resbos->GetECM();
        double t1 = pow(ecm,4)+pow(Q,4)-2*pow(ecm*Q,2)*cosh(2*y);
        return sqrt(t1)/(2*ecm*cosh(y));
    }

    std::vector<double> Calculation::GetPoint(const double &Q, const double &qt, const double &y) {
        std::vector<double> result;
        for(size_t i = 0; i < grid.size(); ++i) {
            size_t QOrder = 4;
            if(grid[i] -> GetXDim() < QOrder) QOrder = grid[i] -> GetXDim();
            size_t yOrder = 4;
            if(grid[i] -> GetYDim() < yOrder) yOrder = grid[i] -> GetYDim();
            size_t qtOrder = 4;
            if(grid[i] -> GetZDim() < qtOrder) qtOrder = grid[i] -> GetZDim();
            result.push_back(grid[i] -> Interpolate(Q,y,qt,QOrder,yOrder,qtOrder));
        }
        return result;
    }

    bool Calculation::SaveGrid() {
        // Parameters of the grid generation
        std::string pdfName = resbos -> GetPDFName(); 
        int iset = resbos -> GetISet();
        std::string pdf = pdfName + "_" + std::to_string(iset);
        double ecm = resbos -> GetECM();

        // Open up info file of all grids, and find the last used number
        std::fstream gridKeys("Grids/gridlist.txt", 
                std::ios::in | std::ios::out | std::ios::app);
        int gridFileNum = 0;

        // Ensure the file is found and open and get the last key
        if(gridKeys.is_open()) {
            std::string line;
            // Get the last line
            gridKeys.seekg(-1, std::ios_base::end);
            if(gridKeys.peek() == '\n') {
                gridKeys.seekg(-1, std::ios_base::cur);
                for(long int i = gridKeys.tellg(); i > 0; --i) {
                    if(gridKeys.peek() == '\n') {
                        gridKeys.get();
                        break;
                    }
                    gridKeys.seekg(i, std::ios_base::beg);
                }
            }
            getline(gridKeys,line);
            try {
                gridFileNum = std::stoi(line.substr(0, line.find(' '))) + 1; 
            } catch(const std::exception &e) {
                gridFileNum = 0;
            }
        } 
        gridKeys.clear();

        // TODO: Switch to libfmt and finish up metadata hashing
        // If the file doesn't exist, we need to had a header line
        gridKeys << std::setprecision(2) << std::left << std::fixed;
        if(gridFileNum == 0) { 
            gridKeys << std::setw(5) << "Key" << std::setw(12) << "Process";
            gridKeys << std::setw(23) << "PDF" << std::setw(10) << "ecm(GeV)";
            gridKeys << std::setw(10) << "GridType" <<  std::setw(9) << "C1(muR)";
            gridKeys << std::setw(9) << "C2(muF)" << std::setw(9) << "C3";
            gridKeys << std::setw(9) << "qMin" << std::setw(9) << "qMax";
            gridKeys << std::setw(9) << "qTMin" << std::setw(9) << "qTMax";
            gridKeys << std::setw(9) << "yMin" << std::setw(9) << "yMax" << '\n';
        }

        // Add information line to the gridlist file
        std::string gridtype;
        double scale1, scale2, scale3;
        if(gType == GridType::Resum) {
            gridtype = "w" + std::to_string(AOrder) + std::to_string(BOrder)
                + std::to_string(COrder);
            if(scheme == Scheme::CFG) gridtype += std::to_string(HOrder);
            scale1 = C1/QCD::B0;
            scale2 = C2;
            scale3 = C3/QCD::B0;
        } else if(gType == GridType::DelSig) {
            gridtype = "delsig" + std::to_string(DeltaSigmaOrder);
            scale1 = muR;
            scale2 = muF;
            if(scale == CentralScale::Q) scale3 = 0;
            else if(scale == CentralScale::MT) scale3 = 1;
        } else if(gType == GridType::Asym) {
            gridtype = "asym" + std::to_string(AsymOrder);
            scale1 = muR;
            scale2 = muF;
            if(scale == CentralScale::Q) scale3 = 0;
            else if(scale == CentralScale::MT) scale3 = 1;
        } else if(gType == GridType::Pert) {
            gridtype = "Pert" + std::to_string(PertOrder);
            scale1 = muR;
            scale2 = muF;
            if(scale == CentralScale::Q) scale3 = 0;
            else if(scale == CentralScale::MT) scale3 = 1;
        } else {
            throw std::runtime_error("Invalid grid type");
        }

        std::string processName = resbos -> GetProcess() -> Name();
        gridKeys << std::setw(5) << gridFileNum << std::setw(12) << processName;
        gridKeys << std::setw(23) << pdf << std::setw(10) << ecm;
        gridKeys << std::setw(10) << gridtype << std::setw(9) << scale1;
        gridKeys << std::setw(9) << scale2 << std::setw(9) << scale3;
        gridKeys << std::setw(9) << qMin << std::setw(9) << qMax;
        gridKeys << std::setw(9) << qTMin << std::setw(9) << qTMax;
        gridKeys << std::setw(9) << yMin << std::setw(9) << yMax << '\n';

        // Create file
#ifdef HAVE_GZIP
        std::string filename = "ResBos_GridFile_" 
            + std::to_string(gridFileNum) + ".out.gz"; 
        ogzstream gridOut(("Grids/" + filename).c_str());
#else
        std::string filename = "ResBos_GridFile_" 
            + std::to_string(gridFileNum) + ".out"; 
        std::ofstream gridOut("Grids/" + filename);
#endif
       
        // Ensure file opened
        if(!gridOut.is_open())
            throw std::runtime_error("Unable to open grid file: " + filename);

        // Write header of the file
        gridOut << std::setprecision(5) << std::fixed << std::left;
        gridOut << std::setw(20) << "Process: " << processName << '\n';
        gridOut << std::setw(20) << "PDF: " << pdfName << '\n';
        gridOut << std::setw(20) << "iSet: " << iset << '\n';
        gridOut << std::setw(20) << "ECM: " << ecm << '\n';
        if(gType == GridType::Resum) {
            gridOut << std::setw(20) << "GridType:" << "Resum" << '\n';
            gridOut << std::setw(20) << "Scheme: ";
            if(scheme == Scheme::CSS) gridOut << "CSS" << '\n';
            else gridOut << "CFG" << '\n';
            gridOut << std::setw(20) << "A Order: " << AOrder << '\n';
            gridOut << std::setw(20) << "B Order: " << BOrder << '\n';
            gridOut << std::setw(20) << "C Order: " << COrder << '\n';
            if(scheme == Scheme::CFG) 
                gridOut << std::setw(20) << "H Order: " << HOrder << '\n';
            gridOut << std::setw(20) << "C1, C2, C3: " 
                << C1 << ", " << C2 << ", " << C3 << '\n';
        } else if(gType == GridType::DelSig) {
            gridOut << std::setw(20) << "GridType:" << "Delta Sigma" << '\n';
            gridOut << std::setw(20) << "Delta Sigma Order: " << DeltaSigmaOrder << '\n';
            gridOut << std::setw(20) << "Central, muR, muF: ";
            if(scale == CentralScale::Q) gridOut << "Q, ";
            else if(scale == CentralScale::MT) gridOut << "MT, ";
            gridOut << muR << ", " << muF << '\n';
        } else if(gType == GridType::Asym) {
            gridOut << std::setw(20) << "GridType:" << "Asymptotic" << '\n';
            gridOut << std::setw(20) << "Asymptotic Order: " << AsymOrder << '\n';
            gridOut << std::setw(20) << "Central, muR, muF: ";
            if(scale == CentralScale::Q) gridOut << "Q, ";
            else if(scale == CentralScale::MT) gridOut << "MT, ";
            gridOut << muR << ", " << muF << '\n';
        } else if(gType == GridType::Pert) {
            gridOut << std::setw(20) << "GridType:" << "Perturbative" << '\n';
            gridOut << std::setw(20) << "Perturbative Order: " << PertOrder << '\n';
            gridOut << std::setw(20) << "Central, muR, muF: ";
            if(scale == CentralScale::Q) gridOut << "Q, ";
            else if(scale == CentralScale::MT) gridOut << "MT, ";
            gridOut << muR << ", " << muF << '\n';
        } else {
            throw std::runtime_error("Invalid grid type");
        }
        gridOut << std::setw(20) << "Q Min/Max: " << qMin << ", " << qMax << '\n';
        gridOut << std::setw(20) << "Qt Min/Max: " << qTMin << ", " << qTMax << '\n';
        gridOut << std::setw(20) << "y Min/Max: " << yMin << ", " << yMax << '\n';
        gridOut << "----------------------------------" << '\n';
        gridOut << std::scientific << std::setprecision(8); 
        gridOut << std::setw(17) << "Q" << std::setw(17) << "Qt" << std::setw(17) << "y";

        size_t nChannels = grid.size();
        for(size_t i = 0; i < nChannels; ++i) {
            if(gType == GridType::Resum)
                gridOut << std::setw(17) << "Resum" + std::to_string(i);
            else if(gType == GridType::DelSig)
                gridOut << std::setw(17) << "DelSig" + std::to_string(i);
            else if(gType == GridType::Asym)
                gridOut << std::setw(17) << "Asym" + std::to_string(i);
            else if(gType == GridType::Pert)
                gridOut << std::setw(17) << "Pert" + std::to_string(i);
        }
        gridOut << '\n';

        // Write out contents of the grid
        for(size_t iQ = 0; iQ < grid.at(0) -> GetXDim(); ++iQ) {
            for(size_t iY = 0; iY < grid.at(0) -> GetYDim(); ++iY) {
                for(size_t iQt = 0; iQt < grid.at(0) -> GetZDim(); ++iQt) {
                    gridOut << std::setw(17) << grid.at(0) -> GetX(iQ);
                    gridOut << std::setw(17) << grid.at(0) -> GetZ(iQt);
                    gridOut << std::setw(17) << grid.at(0) -> GetY(iY);
                    for(size_t iChannel = 0; iChannel < nChannels; ++iChannel) {
                        gridOut << std::setw(17) << grid.at(iChannel) -> FindPoint(iQ,iY,iQt);
                    }
                    gridOut << '\n';
                }
            }
        }

        return true;
    }

    bool Calculation::LoadGrid() {
        // Open the index file to check if a grid has been generated for these set of points 
        std::ifstream gridKeys("Grids/gridlist.txt");
        if(!gridKeys.is_open()) return false;

        // Skip over header line
        std::string line;
        std::getline(gridKeys, line);

        // Load information needed for the comparison to the grid lists
        int key;
        std::string processName = resbos -> GetProcess() -> Name();
        std::string pdfName = resbos -> GetPDFName(); 
        int iset = resbos -> GetISet();
        pdfName += "_" + std::to_string(iset);
        double ecm = resbos -> GetECM();

        std::string gridtype;
        double scale1, scale2, scale3 = 0;
        if(gType == GridType::Resum) {
            gridtype = "w" + std::to_string(AOrder) + std::to_string(BOrder)
                + std::to_string(COrder);
            if(scheme == Scheme::CFG) gridtype += std::to_string(HOrder);
            scale1 = C1/QCD::B0;
            scale2 = C2;
            scale3 = C3/QCD::B0;
        } else if(gType == GridType::DelSig) {
            gridtype = "delsig" + std::to_string(DeltaSigmaOrder);
            scale1 = muR;
            scale2 = muF;
            if(scale == CentralScale::Q) scale3 = 0;
            else if(scale == CentralScale::MT) scale3 = 1;
        } else if(gType == GridType::Asym) {
            gridtype = "asym" + std::to_string(AsymOrder);
            scale1 = muR;
            scale2 = muF;
            if(scale == CentralScale::Q) scale3 = 0;
            else if(scale == CentralScale::MT) scale3 = 1;
        } else if(gType == GridType::Pert) {
            gridtype = "Pert" + std::to_string(PertOrder);
            scale1 = muR;
            scale2 = muF;
            if(scale == CentralScale::Q) scale3 = 0;
            else if(scale == CentralScale::MT) scale3 = 1;
        } else {
            throw std::runtime_error("Invalid grid mode");
        }

        bool found = false;
        while(gridKeys >> key) {
            std::string dummy;
            std::string processName_, pdfName_, gridtype_;
            double scale1_, scale2_, scale3_, ecm_;
            double qMin_, qMax_, qTMin_, qTMax_, yMin_, yMax_;
            gridKeys >> processName_ >> pdfName_ >> ecm_ >> gridtype_ >> scale1_ >> scale2_ >> scale3_;
            gridKeys >> qMin_ >> qMax_ >> qTMin_ >> qTMax_ >> yMin_ >> yMax_;

            // Check each line to see if a valid grid file exists
            if(processName_ != processName) continue;
            if(pdfName_ != pdfName) continue;
            if(std::abs(ecm_-ecm)/ecm > 1E-3) continue;
            if(gridtype_ != gridtype) continue;
            if(std::abs(scale1_-scale1) > 1E-3) continue;
            if(std::abs(scale2_-scale2) > 1E-3) continue;
            if(std::abs(scale3_-scale3) > 1E-3) continue;

            // Check if the grid can still be used because it was generated for a larger region
            if(qTMin == 0) qTMin_ -= 0.1;
            if(qMin_ > qMin) continue;
            if(qMax_ < qMax) continue;
            if(qTMin_ > qTMin) continue;
            if(qTMax_ < qTMax) continue;
            if(yMin_ > yMin) continue;
            if(yMax_ < yMax) continue;

            // Here the grid file key has been found, and the loop can be broken
            found = true;
            break;
        }

        if(!found) return false;

        // Initialize variables for readin
        size_t nChannels = resbos -> GetProcess() -> GetChannels() 
                         * resbos -> GetProcess() -> GetResumAngular();
        if(gType == GridType::Pert) 
            nChannels = resbos -> GetProcess() -> GetChannels() 
                      * resbos -> GetProcess() -> GetNAngular();
        std::vector<double> QGrid, QTGrid, YGrid;
        std::map<size_t,std::vector<double>> values;
        std::string token;

        // If the file is found, we know its key and can construct the name
        std::string filename = "ResBos_GridFile_" 
            + std::to_string(key) + ".out"; 
       
#ifdef HAVE_GZIP
        try{
             // Load file
             igzstream gridIn(("Grids/" + filename + ".gz").c_str());

            // Check to see if the file exists zipped, if not try with an unzipped version
            if(!gridIn.is_open())
                throw std::runtime_error("No gzip file");

            // Skip over the header
            std::stringstream ss;
            ss << std::left;
            ss << std::setw(17) << "Q" << std::setw(17) << "Qt" << std::setw(17) << "y";
            std::string lastHeaderLine = ss.str();
            while(getline(gridIn,line)) {
                if(line.find(lastHeaderLine) != std::string::npos) break;
            }
                
            // Read in the data
            while(gridIn >> token) {
                QGrid.push_back(std::stod(token));
                gridIn >> token;
                QTGrid.push_back(std::stod(token));
                gridIn >> token;
                YGrid.push_back(std::stod(token));
                for(size_t iChannel = 0; iChannel < nChannels; ++iChannel) {
                    gridIn >> token;
                    values[iChannel].push_back(std::stod(token));
                }
            }

        // Catch a non-exsistent gzip file and search to see if there is one unzipped
        } catch (std::exception &e) {
            
            // Load file
            std::ifstream gridIn("Grids/" + filename);

            if(!gridIn.is_open()) {
                throw std::runtime_error("Grid file has been deleted or moved, but not updated in gridlist file");
            }

            // Skip over the header
            std::stringstream ss;
            ss << std::left;
            ss << std::setw(17) << "Q" << std::setw(17) << "Qt" << std::setw(17) << "y";
            std::string lastHeaderLine = ss.str();
            while(getline(gridIn,line)) {
                if(line.find(lastHeaderLine) != std::string::npos) break;
            }
                
            // Read in the data
            while(gridIn >> token) {
                QGrid.push_back(std::stod(token));
                gridIn >> token;
                QTGrid.push_back(std::stod(token));
                gridIn >> token;
                YGrid.push_back(std::stod(token));
                for(size_t iChannel = 0; iChannel < nChannels; ++iChannel) {
                    gridIn >> token;
                    values[iChannel].push_back(std::stod(token));
                }
            }
        }
#else //NOT HAVE_GZIP
        // Load file
        std::ifstream gridIn("Grids/" + filename);

        if(!gridIn.is_open()) {
            throw std::runtime_error("Grid file has been deleted or moved, but not updated in gridlist file");
        }


        // Skip over the header
        std::stringstream ss;
        ss << std::left;
        ss << std::setw(17) << "Q" << std::setw(17) << "Qt" << std::setw(17) << "y";
        std::string lastHeaderLine = ss.str();
        while(getline(gridIn,line)) {
            if(line.find(lastHeaderLine) != std::string::npos) break;
        }
            
        // Read in the data
        while(gridIn >> token) {
            QGrid.push_back(std::stod(token));
            gridIn >> token;
            QTGrid.push_back(std::stod(token));
            gridIn >> token;
            YGrid.push_back(std::stod(token));
            for(size_t iChannel = 0; iChannel < nChannels; ++iChannel) {
                gridIn >> token;
                values[iChannel].push_back(std::stod(token));
            }
        }
#endif //HAVE_GZIP

        // Vectors of unique values of Q, qT, and Y
        std::vector<double> uQGrid(QGrid), uQTGrid(QTGrid), uYGrid(YGrid);

        // Find unique values for Q
        std::sort(uQGrid.begin(), uQGrid.end());
        auto itQ = std::unique(uQGrid.begin(), uQGrid.end());
        uQGrid.resize(static_cast<size_t>(std::distance(uQGrid.begin(),itQ)));

        // Find unique values for QT
        std::sort(uQTGrid.begin(), uQTGrid.end());
        auto itQT = std::unique(uQTGrid.begin(), uQTGrid.end());
        uQTGrid.resize(static_cast<size_t>(std::distance(uQTGrid.begin(),itQT)));

        // Find unique values for Y
        std::sort(uYGrid.begin(), uYGrid.end());
        auto itY = std::unique(uYGrid.begin(), uYGrid.end());
        uYGrid.resize(static_cast<size_t>(std::distance(uYGrid.begin(),itY)));

        // Create grid
        for(size_t iChannel = 0; iChannel < nChannels; ++iChannel) {
            grid[iChannel] = new Utility::Grid3D(uQGrid, uYGrid, uQTGrid);
        }

        // Fill in the grid
        for(size_t i = 0; i < QGrid.size(); ++i) {
            for(size_t iChannel = 0; iChannel < (nChannels); ++iChannel) {
                grid[iChannel] -> AddPoint(QGrid[i], YGrid[i], QTGrid[i], values[iChannel][i]);
            }
        }

        return true;
    }

//    std::pair<BDFMap,BDFMap> ResBos::CalcMap(int pid1, int pid2, double x1, double x2, double Q) const {
//        int order = 0;
//        if((gType==GridType::DelSig && GetDeltaSigmaOrder() > 0) || (gType==GridType::Asym && GetAsymOrder() > 1)) order++;
//        if((gType==GridType::DelSig && GetDeltaSigmaOrder() > 1) || (gType==GridType::Asym && GetAsymOrder() > 2)) order++;
//
//        BDFMap map1 = beams.first.BeamMap(order, pid1, x1, Q);
//        BDFMap map2 = beams.first.BeamMap(order, pid2, x2, Q);
//
//        if(scheme == Scheme::CFG) {
//            // Needed at alpha_s^2 for Asymptotic and alpha_s for DeltaSigma
//            if(order > 0) {
//                double H1 = resbos->GetH1();
//                map1[BDF::C1] += 0.5*H1*map1[BDF::PDF];
//                map2[BDF::C1] += 0.5*H1*map2[BDF::PDF];
//                map1[BDF::C1P1] += 0.5*H1*map1[BDF::P1];
//                map2[BDF::C1P1] += 0.5*H1*map2[BDF::P1];
//
//                // Needed at alpha_s^3 for Asymptotic and alpha_s^2 for DeltaSigma
//                if(order > 1) {
//                    int nf = resbos -> GetNF(Q);
//                    double H2 = resbos -> GetH2(nf);
//                    map1[BDF::C1P2] += 0.5*H1*map1[BDF::P2];
//                    map2[BDF::C1P2] += 0.5*H1*map2[BDF::P2];
//                    map1[BDF::C1P1P1] += 0.5*H1*map1[BDF::P1P1];
//                    map2[BDF::C1P1P1] += 0.5*H1*map2[BDF::P1P1];
//                    map1[BDF::C2] += 0.5*H1*map1[BDF::C1]+0.5*(H2-0.5*pow(H1,2))*map1[BDF::PDF];
//                    map2[BDF::C2] += 0.5*H1*map2[BDF::C1]+0.5*(H2-0.5*pow(H1,2))*map2[BDF::PDF];
//                    map1[BDF::C2P1] += 0.5*H1*map1[BDF::C1P1]+0.5*(H2-0.5*pow(H1,2))*map1[BDF::P1];
//                    map2[BDF::C2P1] += 0.5*H1*map2[BDF::C1P1]+0.5*(H2-0.5*pow(H1,2))*map2[BDF::P1];
//                }
//            }
//        }
//
//        return std::make_pair(map1,map2);
//    }

//    double ResBos::GetConvolution(int hadron, int pid, double x, double Q, Conv c) const { 
//        if(hadron == 0) return beams.first.PDF(Conv2BDF.at(c),pid,x,Q);
//        return beams.second.PDF(Conv2BDF.at(c),pid,x,Q);
//    }

    void Calculation::WriteInp(const std::string &filename, const std::vector<double> &vals) const {
        std::ofstream grid(filename);
        for(const auto &val : vals) {
            grid << val << "\n";
        }
    }

}
