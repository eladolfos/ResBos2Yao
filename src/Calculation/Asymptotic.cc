#include "ResBos/Asymptotic.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/Process.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/ThreadPool.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {
    using namespace QCD;
    REGISTER_CALC(Asymptotic)

    // Initialize the asymptotic class;
    void Asymptotic::Initialize(IO::Settings *s, std::shared_ptr<ResBos> r) {
        gType = GridType::Asym;
        SetAsymOrder(s -> GetSettingSizet("AsymOrder"));
        SetMuF(s -> GetSettingDouble("muF"));
        SetMuR(s -> GetSettingDouble("muR"));
        Calculation::Initialize(s,r);
        if(s -> GetSettingString("scale") == "Q") scale = CentralScale::Q;
        else if(s -> GetSettingString("scale") == "MT") scale = CentralScale::MT;
        else throw std::runtime_error("Asymptotic: Invalid scale choice: " + s -> GetSettingString("scale"));
        DLOG_F(INFO,"Asymptotic Order: %zu", GetAsymOrder());
        DLOG_F(INFO,"Asymptotic Scale: %s", s -> GetSettingString("scale").c_str());
    }

    // Driver function to get Asymptotic piece
    std::vector<double> Asymptotic::GetCalc(const double &Q, const double &qt, const double &y) {
        std::vector<double> asym(resbos -> GetProcess() -> GetChannels() 
                               * resbos -> GetProcess() -> GetResumAngular(), 0);
        double ecm = resbos -> GetECM();

        // Ensure that the rapidity is in a physically allowed region
        if(std::abs(y) > GetYMax(Q,qt)) return asym;

        double x1, x2;
        if(!KinCorr) {
            x1 = Q/ecm*exp(y);
            x2 = Q/ecm*exp(-y);
        } else {
            x1 = sqrt(Q*Q+qt*qt)/ecm*exp(y);
            x2 = sqrt(Q*Q+qt*qt)/ecm*exp(-y);
        }

        // Ensure that the x values are valid
        if(x1 > 1 || x2 > 1) {
            return asym;
        }

        double mu = Q;
        if(scale == CentralScale::MT) mu = sqrt(Q*Q+qt*qt);

        //double coupl = resbos -> GetProcess() -> GetCoupling(GetMuR()*mu);
        double coupl = resbos -> GetProcess() -> GetCoupling(GetMuR()*Q);//yfu
        double alpi = resbos -> GetAlpi(GetMuR()*mu);

        asym = CalcTerms(x1,x2,Q,qt);
        for(size_t i = 0; i < asym.size(); i++) {
            asym[i] *= hbarc2*alpi/qt/pow(ecm,2)*coupl;
            DLOG_F(INFO,"Asym(%zu,%f,%f,%f) = %e",i,Q,qt,y,asym[i]);
        }
            
        return asym;
    }

    std::vector<double> Asymptotic::CalcTerms(const double &x1, const double &x2,
                                              const double &Q, const double &qt) const {
        double dlnQ = 2*log(Q/qt);
        double mu = Q;
        if(scale == CentralScale::MT) mu = sqrt(Q*Q+qt*qt);
        size_t nf = resbos -> GetNF(GetMuR()*mu); 
        double lnMuR, lnMuF;

        std::vector<double> Asym;
        double alpi = resbos -> GetAlpi(GetMuR()*mu);
        std::pair<Beam::Beam*,Beam::Beam*> beams = resbos -> GetBeams();
        for(size_t i = 0; i < resbos -> GetProcess() -> GetChannels(); i++) {
            double symmetric = 0, asymmetric = 0;
            for(auto parton : resbos -> GetProcess() -> GetPartons(i)) {
                double q_qb=0, qb_q=0;
                double ckm = 1.0;
                if(resbos -> GetProcess() -> Name() == "WPlus"
                || resbos -> GetProcess() -> Name() == "WMinus") {
                    ckm = resbos -> GetProcess() -> GetEW() -> GetCKM(parton.first, parton.second);
                }
                ConvMap cMap11=beams.first->BeamMap(GetAsymOrder()-1,parton.first,x1,GetMuF()*mu),
                        cMap12=beams.first->BeamMap(GetAsymOrder()-1,parton.second,x1,GetMuF()*mu),
                        cMap21=beams.second->BeamMap(GetAsymOrder()-1,parton.first,x2,GetMuF()*mu),
                        cMap22=beams.second->BeamMap(GetAsymOrder()-1,parton.second,x2,GetMuF()*mu);

                // Alpha_s contribution to asymptotic piece
                q_qb += ckm*(C11(nf,cMap11,cMap22)*dlnQ+C10(nf,cMap11,cMap22));
                qb_q += ckm*(C11(nf,cMap12,cMap21)*dlnQ+C10(nf,cMap12,cMap21));

                // Alpha_s^2 contribution to asymptotic piece
                if(GetAsymOrder() > 1) {
                    q_qb += (C23(nf,cMap11,cMap22)*pow(dlnQ,3)
                            +C22(nf,cMap11,cMap22)*pow(dlnQ,2)
                            +C21(nf,cMap11,cMap22)*dlnQ
                            +C20(nf,cMap11,cMap22))*alpi*ckm;
                    qb_q += (C23(nf,cMap12,cMap21)*pow(dlnQ,3)
                            +C22(nf,cMap12,cMap21)*pow(dlnQ,2)
                            +C21(nf,cMap12,cMap21)*dlnQ
                            +C20(nf,cMap12,cMap21))*alpi*ckm;

                    if(GetMuF() != 1 || GetMuR() != 1 || scale == CentralScale::MT) {
                        lnMuR = -2*log(GetMuR()*mu/Q);
                        lnMuF = -2*log(GetMuF()*mu/Q);
                        q_qb += (C21log(nf,cMap11,cMap22,lnMuR,lnMuF)*dlnQ
                                +C20log(nf,cMap11,cMap22,lnMuR,lnMuF))*alpi*ckm;
                        qb_q += (C21log(nf,cMap21,cMap12,lnMuR,lnMuF)*dlnQ
                                +C20log(nf,cMap21,cMap12,lnMuR,lnMuF))*alpi*ckm;
                    }

                    // Alpha_s^3 contribution to asymptotic piece
                    if(GetAsymOrder() > 2) {
                        q_qb += (C35(nf,cMap11,cMap22)*pow(dlnQ,5)
                                +C34(nf,cMap11,cMap22)*pow(dlnQ,4)
                                +C33(nf,cMap11,cMap22)*pow(dlnQ,3)
                                +C32(nf,cMap11,cMap22)*pow(dlnQ,2)
                                +C31(nf,cMap11,cMap22)*dlnQ
                                +C30(nf,Q,cMap11,cMap22))*pow(alpi,2)*ckm;
                        qb_q += (C35(nf,cMap21,cMap12)*pow(dlnQ,5)
                                +C34(nf,cMap21,cMap12)*pow(dlnQ,4)
                                +C33(nf,cMap21,cMap12)*pow(dlnQ,3)
                                +C32(nf,cMap21,cMap12)*pow(dlnQ,2)
                                +C31(nf,cMap21,cMap12)*dlnQ
                                +C30(nf,Q,cMap21,cMap12))*pow(alpi,2)*ckm;
                        if(GetMuF() != 1 || GetMuR() != 1 || scale == CentralScale::MT) {
                            lnMuR = -2*log(GetMuR()*mu/Q);
                            lnMuF = -2*log(GetMuF()*mu/Q);
                            q_qb += (C33log(nf,cMap11,cMap22,lnMuR,lnMuF)*pow(dlnQ,3) 
                                    +C32log(nf,cMap11,cMap22,lnMuR,lnMuF)*pow(dlnQ,2)
                                    +C31log(nf,cMap11,cMap22,lnMuR,lnMuF)*dlnQ
                                    +C30log(nf,cMap11,cMap22,lnMuR,lnMuF))*pow(alpi,2)*ckm;
                            qb_q += (C33log(nf,cMap21,cMap12,lnMuR,lnMuF)*pow(dlnQ,3)
                                    +C32log(nf,cMap21,cMap12,lnMuR,lnMuF)*pow(dlnQ,2)
                                    +C31log(nf,cMap21,cMap12,lnMuR,lnMuF)*dlnQ
                                    +C30log(nf,cMap21,cMap12,lnMuR,lnMuF))*pow(alpi,2)*ckm;
                        }
                    }
                }  

                if(parton.first == parton.second) {
                    q_qb /= 2.0;
                    qb_q /= 2.0;
                }

                symmetric += resbos->GetProcess()->Charge(parton.first,0)*(q_qb+qb_q);
                asymmetric += resbos->GetProcess()->Charge(parton.first,1)*(q_qb-qb_q);

            }

            // Calculate the symmetric and antisymmetric pieces of the asymptotic piece
            Asym.push_back(symmetric);
            if(resbos -> GetProcess() -> GetResumAngular() > 1) Asym.push_back(asymmetric);
        }

        return Asym;
    }
            
    typedef Beam::BDF cv;
    // alpha_s Log Asymptotic function
    double Asymptotic::C11(const size_t&, const ConvMap &cmap1, const ConvMap &cmap2) const {
        return 2*GetA1()*cmap1.at(cv::PDF)*cmap2.at(cv::PDF);
    }

    // alpha_s Log^0 Asymptotic function
    double Asymptotic::C10(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        return 2*GetB1(nf)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF);
    }

    // alpha_s^2 Log^3 Asymptotic function
    double Asymptotic::C23(const size_t&, const ConvMap &cmap1, const ConvMap &cmap2) const {
        return -pow(GetA1(),2)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF);
    }

    // alpha_s^2 Log^2 Asymptotic function
    double Asymptotic::C22(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();

        return A1*(-3*GetB1(nf)+2*beta0(nf))*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             - 1.5*A1*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF));
    }

    // alpha_s^2 Log Asymptotic function
    double Asymptotic::C21(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();
        double B1 = GetB1(nf);
        double A2 = GetA2(nf);

        return 2*A1*(cmap1.at(cv::PDF)*cmap2.at(cv::C1)+cmap2.at(cv::PDF)*cmap1.at(cv::C1))
             + 2*(-pow(B1,2)+beta0(nf)*B1+A2)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + (beta0(nf)-2*B1)*(cmap1.at(cv::PDF)*cmap2.at(cv::P1)
                                +cmap2.at(cv::PDF)*cmap1.at(cv::P1))
             - cmap1.at(cv::P1)*cmap2.at(cv::P1)
             -0.5*(cmap1.at(cv::P1P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1P1)*cmap1.at(cv::PDF));
    }

    // alpha_s^2 Log^0 Asymptotic function
    double Asymptotic::C20(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();
        double B1 = GetB1(nf);
//        double A2 = GetA2(nf);
        double B2 = GetB2(nf);

        if(GetScheme() == Scheme::CFG) {
            B2 = B2+beta0(nf)*resbos -> GetProcess() -> H1();
        }

        return (4*ZETA3*pow(A1,2)+2*B2)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + 2*(B1-beta0(nf))*(cmap1.at(cv::C1)*cmap2.at(cv::PDF)
                                +cmap2.at(cv::C1)*cmap1.at(cv::PDF))
             + cmap1.at(cv::C1)*cmap2.at(cv::P1)+cmap2.at(cv::C1)*cmap1.at(cv::P1)
             + cmap1.at(cv::C1P1)*cmap2.at(cv::PDF)+cmap2.at(cv::C1P1)*cmap1.at(cv::PDF)
             + 0.5*(cmap1.at(cv::P2)*cmap2.at(cv::PDF)+cmap2.at(cv::P2)*cmap1.at(cv::PDF));
    }

    // alpha_s^3 Log^5 Asymptotic function
    double Asymptotic::C35(const size_t&, const ConvMap &cmap1, const ConvMap &cmap2) const {
        return 0.25*pow(GetA1(),3)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF);
    }

    // alpha_s^3 Log^4 Asymptotic function
    double Asymptotic::C34(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();
        double B1 = GetB1(nf);

        return (5.0/4.0*B1-5.0/3.0*beta0(nf))*pow(A1,2)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + 5.0/8.0*pow(A1,2)*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)
                                 +cmap2.at(cv::P1)*cmap1.at(cv::PDF));
    }


    // alpha_s^3 Log^3 Asymptotic function
    double Asymptotic::C33(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();
        double A2 = GetA2(nf);
        double B1 = GetB1(nf);

        return A1*((-2*A2+2*pow(B1,2)-14.0/3.0*beta0(nf)*B1
                    +2*pow(beta0(nf),2))*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + (2*B1-7.0*beta0(nf)/3.0)*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)
                                        +cmap2.at(cv::P1)*cmap1.at(cv::PDF))
             + (cmap1.at(cv::P1)*cmap2.at(cv::P1)
                 +0.5*cmap1.at(cv::P1P1)*cmap2.at(cv::PDF)
                 +0.5*cmap2.at(cv::P1P1)*cmap1.at(cv::PDF)))
             - pow(A1,2)*(cmap1.at(cv::C1)*cmap2.at(cv::PDF)+cmap2.at(cv::C1)*cmap1.at(cv::PDF));
    }

    // alpha_s^3 Log^2 Asymptotic function
    double Asymptotic::C32(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1(); 
        double A2 = GetA2(nf); 
        double B1 = GetB1(nf); 
        double B2 = GetB2(nf); 

        if(GetScheme() == Scheme::CFG) {
            B2 = B2+beta0(nf)*resbos -> GetProcess() -> H1();
        }

        return (-3*B1*A2-3*A1*B2-10*ZETA3*pow(A1,3)+2*beta1(nf)*A1-3*beta0(nf)*pow(B1,2)
                +pow(B1,3)+2*pow(beta0(nf),2)*B1
                +4*beta0(nf)*A2)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + (3.0/2.0*pow(B1,2)-3*beta0(nf)*B1-3.0/2.0*A2+pow(beta0(nf),2))
                *(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF))
             - 3.0/4.0*A1*(cmap1.at(cv::P2)*cmap2.at(cv::PDF)+cmap2.at(cv::P2)*cmap1.at(cv::PDF))
             + (5*beta0(nf)*A1-3*A1*B1)
                *(cmap1.at(cv::C1)*cmap2.at(cv::PDF)+cmap2.at(cv::C1)*cmap1.at(cv::PDF))
             - 3.0/2.0*A1*(cmap1.at(cv::C1P1)*cmap2.at(cv::PDF)
                          +cmap1.at(cv::C1P1)*cmap2.at(cv::PDF)
                     +cmap1.at(cv::C1)*cmap2.at(cv::P1)+cmap2.at(cv::C1)*cmap1.at(cv::P1))
             + 3.0/2.0*(B1-beta0(nf))*(cmap1.at(cv::P1)*cmap2.at(cv::P1)
                     +0.5*(cmap1.at(cv::P1P1)*cmap2.at(cv::PDF)
                          +cmap2.at(cv::P1P1)*cmap1.at(cv::PDF)))
             + 1.0/8.0*(cmap1.at(cv::P1P1P1)*cmap2.at(cv::PDF)
                       +cmap2.at(cv::P1P1P1)*cmap1.at(cv::PDF)
                     +3*cmap1.at(cv::P1P1)*cmap2.at(cv::P1)+3*cmap2.at(cv::P1P1)*cmap1.at(cv::P1));
    }

    // alpha_s^3 Log Asymptotic function
    double Asymptotic::C31(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();
        double A2 = GetA2(nf); 
        double A3 = GetA3(nf); 
        double B1 = GetB1(nf); 
        double B2 = GetB2(nf); 

        if(GetScheme() == Scheme::CFG) {
            B2 = B2+beta0(nf)*resbos -> GetProcess() -> H1();
        }
       
        double C31Result = (-4*B1*B2-20*ZETA3*pow(A1,2)*B1+80*beta0(nf)/3.0*ZETA3*pow(A1,2)
                +2*beta1(nf)*B1+4*beta0(nf)*B2+2*A3)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             - (10*ZETA3*pow(A1,2)+2*B2-beta1(nf))
                *(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF))
             - 0.25*(cmap1.at(cv::P2P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P2P1)*cmap1.at(cv::PDF)
                     +cmap1.at(cv::P1P2)*cmap2.at(cv::PDF)+cmap2.at(cv::P1P2)*cmap1.at(cv::PDF))
             - 0.5*(cmap1.at(cv::P2)*cmap2.at(cv::P1)+cmap2.at(cv::P2)*cmap1.at(cv::P1))
             + (beta0(nf)-B1)*(cmap1.at(cv::P2)*cmap2.at(cv::PDF)
                              +cmap2.at(cv::P2)*cmap1.at(cv::PDF))
             + (6*beta0(nf)*B1-2*pow(B1,2)+2*A2-4*pow(beta0(nf),2))
                *(cmap1.at(cv::C1)*cmap2.at(cv::PDF)+cmap2.at(cv::C1)*cmap1.at(cv::PDF))
             + (3*beta0(nf)-2*B1)*(cmap1.at(cv::C1P1)*cmap2.at(cv::PDF)
                                  +cmap2.at(cv::C1P1)*cmap1.at(cv::PDF)
                     +cmap1.at(cv::C1)*cmap2.at(cv::P1)+cmap2.at(cv::C1)*cmap1.at(cv::P1))
             + 2*A1*(cmap1.at(cv::C2)*cmap2.at(cv::PDF)+cmap2.at(cv::C2)*cmap1.at(cv::PDF)
                     +cmap1.at(cv::C1)*cmap2.at(cv::C1))
             - (cmap1.at(cv::C1P1)*cmap2.at(cv::P1)+cmap2.at(cv::C1P1)*cmap1.at(cv::P1))
             - 0.5*(cmap1.at(cv::C1)*cmap2.at(cv::P1P1)+cmap2.at(cv::C1)*cmap1.at(cv::P1P1)
                     +cmap1.at(cv::C1P1P1)*cmap2.at(cv::PDF)
                     +cmap2.at(cv::C1P1P1)*cmap1.at(cv::PDF));
        if(resbos -> GetProcess() -> GetInitState() == InitialState::gg) 
            C31Result += 2*A1*cmap1.at(cv::G1)*cmap2.at(cv::G1);

        return C31Result;
    }

    // alpha_s^3 Log^0 Asymptotic function
    double Asymptotic::C30(const size_t& nf, const double &Q, 
                           const ConvMap &cmap1, const ConvMap &cmap2) const {
        double A1 = GetA1();
        double A2 = GetA2(nf); 
//        double A3 = GetA3(nf); 
        double B1 = GetB1(nf); 
        double B2 = GetB2(nf); 
        double B3 = GetB3(nf); 

        if(GetScheme() == Scheme::CFG) {
            B2 = B2+beta0(nf)*resbos -> GetProcess() -> H1();
            B3 = B3+beta1(nf)*resbos -> GetProcess() -> H1()+2*beta0(nf)*(resbos -> GetProcess() -> H2(Q)-0.5*pow(resbos -> GetProcess() -> H1(),2));
        }

        double C30Result =
             (8*ZETA3*A1*A2-12*ZETA5*pow(A1,3)-8*ZETA3*A1*pow(B1,2)+
              56*beta0(nf)/3.0*ZETA3*A1*B1-8*ZETA3*pow(beta0(nf),2)*A1+2*B3)
                *cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
             + (28*beta0(nf)/3.0*ZETA3*A1-8*ZETA3*A1*B1)
                *(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF))
             - 2*ZETA3*A1*(cmap1.at(cv::P1P1)*cmap2.at(cv::PDF)
                          +cmap2.at(cv::P1P1)*cmap1.at(cv::PDF)
                     +2*cmap1.at(cv::P1)*cmap2.at(cv::P1))
             + (4*ZETA3*pow(A1,2)+2*B2-2*beta1(nf))
                *(cmap1.at(cv::C1)*cmap2.at(cv::PDF)+cmap2.at(cv::C1)*cmap1.at(cv::PDF))
             + 0.5*(cmap1.at(cv::C1)*cmap2.at(cv::P2)+cmap2.at(cv::C1)*cmap1.at(cv::P2)
                     +cmap1.at(cv::C1P2)*cmap2.at(cv::PDF)+cmap2.at(cv::C1P2)*cmap1.at(cv::PDF))
             + (2*B1-4*beta0(nf))*(cmap1.at(cv::C2)*cmap2.at(cv::PDF)
                                  +cmap2.at(cv::C2)*cmap1.at(cv::PDF))
             + (cmap1.at(cv::C2P1)*cmap2.at(cv::PDF)+cmap2.at(cv::C2P1)*cmap1.at(cv::PDF)
                     +cmap1.at(cv::C2)*cmap2.at(cv::P1)+cmap2.at(cv::C2)*cmap1.at(cv::P1))
             + (2*B1-4*beta0(nf))*(cmap1.at(cv::C1)*cmap2.at(cv::C1))
             + (cmap1.at(cv::C1P1)*cmap2.at(cv::C1)+cmap2.at(cv::C1P1)*cmap1.at(cv::C1))
             + 0.25*(cmap1.at(cv::P3)*cmap2.at(cv::PDF)+cmap2.at(cv::P3)*cmap1.at(cv::PDF));
    
        if(resbos -> GetProcess() -> GetInitState() == InitialState::gg) 
            C30Result += (2*B1-4*beta0(nf))*cmap1.at(cv::G1)*cmap2.at(cv::G1)
                      + (cmap1.at(cv::G1P1)*cmap2.at(cv::G1)+cmap2.at(cv::G1P1)*cmap1.at(cv::G1));

        return C30Result;
              
    }

    // alpha_s^2 log Asymptotic muF and muR dependence
    double Asymptotic::C21log(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2,
                              const double &lnMuR, const double &lnMuF) const {
        double A1 = GetA1();

        return -2*A1*beta0(nf)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)*lnMuR
            +lnMuF*A1*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF));
    }

    // alpha_s^2 log^0 Asymptotic muF and muR dependence
    double Asymptotic::C20log(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2, 
                              const double &lnMuR, const double &lnMuF) const {
        double B1 = GetB1(nf);

        return lnMuR*(-2*B1*beta0(nf)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
                -beta0(nf)*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF)))
             + lnMuF*(B1*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF))
                     +cmap1.at(cv::P1)*cmap2.at(cv::P1)
                     +0.5*(cmap1.at(cv::P1P1)*cmap2.at(cv::PDF)
                          +cmap2.at(cv::P1P1)*cmap1.at(cv::PDF)));
    }

    // alpha_s^3 log^3 Asymptotic muF and muR dependence
    double Asymptotic::C33log(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2, 
                              const double &lnMuR, const double &lnMuF) const {
        double A1 = GetA1();

        return pow(A1,2)*(lnMuR*2*beta0(nf)*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
                        - 0.5*lnMuF*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)
                                    +cmap2.at(cv::P1)*cmap1.at(cv::PDF)));
    }

    // alpha_s^3 log^2 Asymptotic muF and muR dependence
    double Asymptotic::C32log(const size_t& nf, const ConvMap &cmap1, const ConvMap &cmap2, 
                              const double &lnMuR, const double &lnMuF) const {
        double A1 = GetA1();
        double B1 = GetB1(nf);

        return A1*(lnMuR*((6*B1*beta0(nf)-4*pow(beta0(nf),2))*cmap1.at(cv::PDF)*cmap2.at(cv::PDF)
                        +3*beta0(nf)*(cmap1.at(cv::P1)*cmap2.at(cv::PDF)
                                     +cmap2.at(cv::P1)*cmap1.at(cv::PDF)))
                 + lnMuF*((beta0(nf)-3.0/2.0*B1)*(
                        cmap1.at(cv::P1)*cmap2.at(cv::PDF)+cmap2.at(cv::P1)*cmap1.at(cv::PDF))
                        -3.0/2.0*(cmap1.at(cv::P1)*cmap2.at(cv::P1)
                        +0.5*cmap1.at(cv::P1P1)*cmap2.at(cv::PDF)
                        +0.5*cmap2.at(cv::P1P1)*cmap1.at(cv::PDF))));
    }

    // alpha_s^3 log Asymptotic muF and muR dependence
    double Asymptotic::C31log(const size_t&, const ConvMap&, const ConvMap&, 
                              const double&, const double&) const {
        return 0;
    }

    // alpha_s^3 log^0 Asymptotic muF and muR dependence
    double Asymptotic::C30log(const size_t&, const ConvMap&, const ConvMap&,
                              const double&, const double&) const {
        return 0;
    }

}
