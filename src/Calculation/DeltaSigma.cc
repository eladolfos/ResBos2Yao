#include "ResBos/DeltaSigma.hh"
#include "ResBos/Process.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/ThreadPool.hh"

#include "ResBos/loguru.hpp"

namespace ResBos {
    using namespace QCD;
    using namespace Beam;
    REGISTER_CALC(DeltaSigma)

    // Initialize the DeltaSigma class;
    void DeltaSigma::Initialize(IO::Settings *s, std::shared_ptr<ResBos> resbos_) {
        gType = GridType::DelSig;
        SetDeltaSigmaOrder(s->GetSettingSizet("DeltaSigmaOrder"));
        SetMuF(s->GetSettingDouble("muF"));
        SetMuR(s->GetSettingDouble("muR"));
        Calculation::Initialize(s,resbos_);
        if(s -> GetSettingString("scale") == "Q") scale = CentralScale::Q;
        else if(s -> GetSettingString("scale") == "MT") scale = CentralScale::MT;
        else throw std::runtime_error("Delta Sigma: Invalid scale choice: " + s -> GetSettingString("scale"));
        DLOG_F(INFO,"Delta Sigma Order: %zu", GetDeltaSigmaOrder());
        DLOG_F(INFO,"Delta Sigma Scale: %s", s -> GetSettingString("scale").c_str());
    }

    std::vector<Conv> DeltaSigma::GetNeededConvs() const{
        std::vector<Conv> neededGrids;
        if(GetDeltaSigmaOrder()>0) {
            neededGrids.push_back(Conv::C1);
            neededGrids.push_back(Conv::C1P1);
            if(GetDeltaSigmaOrder()>1) {
                neededGrids.push_back(Conv::C2);
                neededGrids.push_back(Conv::C1P1P1);
                neededGrids.push_back(Conv::C1P2);
            }
        }
        return neededGrids;
    }

    // Driver function to get DeltaSigma piece
    std::vector<double> DeltaSigma::GetCalc(const double &Q, const double &qt, const double &y) {
        std::vector<double> delSig(resbos -> GetProcess() -> GetChannels() * resbos -> GetProcess() -> GetResumAngular(), 0);
        double ecm = resbos -> GetECM();

        // Ensure that the rapidity is in a physically allowed region
        if(std::abs(y) > GetYMax(Q,qt)) return delSig;

        double x1, x2;
        if(!KinCorr) {
            x1 = Q/ecm*exp(y);
            x2 = Q/ecm*exp(-y);
        } else {
            x1 = sqrt(Q*Q+qt*qt)/ecm*exp(y);
            x2 = sqrt(Q*Q+qt*qt)/ecm*exp(-y);
        }

        if(x1 > 1 || x2 > 1) {
            return delSig;
        }

        double mu = Q;
        if(scale == CentralScale::MT) mu = sqrt(Q*Q+qt*qt);
        double coupl = resbos -> GetProcess() -> GetCoupling(GetMuR()*mu);

        delSig = CalcTerms(x1,x2,Q,qt);
        for(size_t i = 0; i < delSig.size(); i++) {
            delSig[i] *= hbarc2/pow(ecm,2)*coupl;
            DLOG_F(INFO,"DelSig(%zu,%f,%f,%f) = %f",i,Q,qt,y,delSig[i]);
        }
        return delSig;
    }

    // Calculation of the logarithm dependent terms for DeltaSigma
    std::vector<double> DeltaSigma::CalcTerms(const double &x1, const double &x2,
                                              const double &Q, const double &qt) const {
        double dlnQ = 2*log(Q/qt);
        double mu = Q;
        if(scale == CentralScale::MT) mu = sqrt(Q*Q+qt*qt);
        size_t nf = resbos -> GetNF(GetMuR()*mu); 
        double lnMuR, lnMuF;

        std::vector<double> DelSig;
        double alpi = resbos->GetAlpi(GetMuR()*mu);
        size_t order = GetDeltaSigmaOrder();
        std::pair<Beam::Beam*, Beam::Beam*> beams = resbos -> GetBeams();
        for(size_t i  = 0; i < resbos -> GetProcess() -> GetChannels(); i++) {
            double symmetric = 0, asymmetric = 0;
            for(auto parton : resbos -> GetProcess() -> GetPartons(i)) {
                double q_qb=0, qb_q=0;
                double ckm = 1.0;
                if(resbos -> GetProcess() -> Name() == "WPlus"
                || resbos -> GetProcess() -> Name() == "WMinus") {
                    ckm = resbos -> GetProcess() -> GetEW() -> GetCKM(parton.first, parton.second);
                }
                ConvMap cMap11 = beams.first -> BeamMap(order, parton.first, x1, GetMuF()*mu),
                        cMap12 = beams.first -> BeamMap(order, parton.second, x1, GetMuF()*mu),
                        cMap21 = beams.second -> BeamMap(order, parton.first, x2, GetMuF()*mu),
                        cMap22 = beams.second -> BeamMap(order, parton.second, x2, GetMuF()*mu);

                // Leading order contribution to DeltaSigma piece
                q_qb += ckm*cMap11[BDF::PDF]*cMap22[BDF::PDF];
                qb_q += ckm*cMap12[BDF::PDF]*cMap21[BDF::PDF];
                if(order > 0) {
                    // Alpha_s contribution to DeltaSigm piece
                    q_qb += (V12(nf,cMap11,cMap22)*pow(dlnQ,2)
                           + V11(nf,cMap11,cMap22)*dlnQ
                           + V10(nf,cMap11,cMap22))*alpi*ckm;
                    qb_q += (V12(nf,cMap21,cMap12)*pow(dlnQ,2)
                           + V11(nf,cMap21,cMap12)*dlnQ
                           + V10(nf,cMap21,cMap12))*alpi*ckm;

                    if(GetMuF() != 1 || GetMuR() != 1 || scale == CentralScale::MT) {
                        lnMuR = -2*log(GetMuR()*mu/Q);
                        lnMuF = -2*log(GetMuF()*mu/Q);
                        q_qb += V10log(nf,cMap11,cMap22,lnMuR,lnMuF)*alpi*ckm;
                        qb_q += V10log(nf,cMap21,cMap12,lnMuR,lnMuF)*alpi*ckm;
                    }

                    // Alpha_s^2 contribution to DeltaSigma piece
                    if(GetDeltaSigmaOrder() > 1) {
                        q_qb += (V24(nf,cMap11,cMap22)*pow(dlnQ,4)
                                +V23(nf,cMap11,cMap22)*pow(dlnQ,3)
                                +V22(nf,cMap11,cMap22)*pow(dlnQ,2)
                                +V21(nf,cMap11,cMap22)*dlnQ
                                +V20(nf,cMap11,cMap22))*pow(alpi,2)*ckm;
                        qb_q += (V24(nf,cMap21,cMap12)*pow(dlnQ,4)
                                +V23(nf,cMap21,cMap12)*pow(dlnQ,3)
                                +V22(nf,cMap21,cMap12)*pow(dlnQ,2)
                                +V21(nf,cMap21,cMap12)*dlnQ
                                +V20(nf,cMap21,cMap12))*pow(alpi,2)*ckm;

                        if(GetMuF() != 1 || GetMuR() != 1 || scale == CentralScale::MT) {
                            lnMuR = -2*log(GetMuR()*mu/Q);
                            lnMuF = -2*log(GetMuF()*mu/Q);
                            q_qb += (V22log(nf,cMap11,cMap22,lnMuR,lnMuF)*pow(dlnQ,2)
                                    +V21log(nf,cMap11,cMap22,lnMuR,lnMuF)*dlnQ
                                    +V20log(nf,cMap11,cMap22,lnMuR,lnMuF))*pow(alpi,2)*ckm;
                            qb_q += (V22log(nf,cMap21,cMap21,lnMuR,lnMuF)*pow(dlnQ,2)
                                    +V21log(nf,cMap21,cMap21,lnMuR,lnMuF)*dlnQ
                                    +V20log(nf,cMap21,cMap21,lnMuR,lnMuF))*pow(alpi,2)*ckm;
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

            // Calculate the symmetric and antisymmetric pieces of the DeltaSigma piece
            DelSig.push_back(symmetric);
            if(resbos -> GetProcess() -> GetResumAngular() > 1) DelSig.push_back(asymmetric);
        }

        return DelSig;
    }

    // alpha_s Log^2 DeltaSigma function
    double DeltaSigma::V12(const size_t&, const ConvMap& cmap1, const ConvMap& cmap2) const {
        return -0.5*GetA1()*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF);
    }

    // alpha_s Log DeltaSigma function
    double DeltaSigma::V11(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2) const {
        return -GetB1(nf)*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
               - 0.5*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)+cmap2.at(BDF::P1)*cmap1.at(BDF::PDF));
    }

    // alpha_s Log^0 DeltaSigma function
    double DeltaSigma::V10(const size_t&, const ConvMap& cmap1, const ConvMap& cmap2) const {
        return cmap1.at(BDF::C1)*cmap2.at(BDF::PDF)
              +cmap2.at(BDF::C1)*cmap1.at(BDF::PDF);
    }

    // alpha_s^2 Log^4 DeltaSigma function
    double DeltaSigma::V24(const size_t&, const ConvMap& cmap1, const ConvMap& cmap2) const {
        double A1 = GetA1();

        return 1.0/8.0*pow(A1,2)*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF);
    }

    // alpha_s^2 Log^3 DeltaSigma function
    double DeltaSigma::V23(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2) const {
        double A1 = GetA1();
        double B1 = GetB1(nf);

        return (0.5*A1*B1-1.0/3.0*beta0(nf)*A1)*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
            + 0.25*A1*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)+cmap2.at(BDF::P1)*cmap1.at(BDF::PDF));
    }

    // alpha_s^2 Log^2 DeltaSigma function
    double DeltaSigma::V22(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2) const {
        double A1 = GetA1();
        double A2 = GetA2(nf);
        double B1 = GetB1(nf);

        return 0.5*(pow(B1,2)-beta0(nf)*B1-A2)*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
            - 0.5*A1*(cmap1.at(BDF::C1)*cmap2.at(BDF::PDF)+cmap2.at(BDF::C1)*cmap1.at(BDF::PDF))
            + 0.25*(2*B1-beta0(nf))*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)
                                    +cmap2.at(BDF::P1)*cmap1.at(BDF::PDF))
            + 0.25*(0.5*cmap1.at(BDF::P1P1)*cmap2.at(BDF::PDF)
                 +0.5*cmap2.at(BDF::P1P1)*cmap1.at(BDF::PDF)+cmap1.at(BDF::P1)*cmap2.at(BDF::P1));
    }

    // alpha_s^2 Log DeltaSigma function
    double DeltaSigma::V21(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2) const {
        double A1 = GetA1();
//        double A2 = GetA2(nf);
        double B1 = GetB1(nf);
        double B2 = GetB2(nf);

        if(GetScheme() == Scheme::CFG) {
            B2 = B2+beta0(nf)*resbos -> GetProcess() -> H1();
        }

        return (-2*ZETA3*pow(A1,2)-B2)*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
            + (beta0(nf)-B1)*(cmap1.at(BDF::C1)*cmap2.at(BDF::PDF)
                             +cmap2.at(BDF::C1)*cmap1.at(BDF::PDF))
            - 0.5*(cmap1.at(BDF::C1)*cmap2.at(BDF::P1)+cmap2.at(BDF::C1)*cmap1.at(BDF::P1)
                    +cmap1.at(BDF::C1P1)*cmap2.at(BDF::PDF)+cmap2.at(BDF::C1P1)*cmap1.at(BDF::PDF))
            - 0.25*(cmap1.at(BDF::P2)*cmap2.at(BDF::PDF)+cmap2.at(BDF::P2)*cmap1.at(BDF::PDF));
    }

    // alpha_s^2 Log^0 DeltaSigma function
    double DeltaSigma::V20(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2) const {
        double A1 = GetA1();
        double B1 = GetB1(nf);

        double result =
            ZETA3*A1*(-2*B1+4.0/3.0*beta0(nf))*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
            - ZETA3*A1*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)+cmap2.at(BDF::P1)*cmap1.at(BDF::PDF))
            + cmap1.at(BDF::C1)*cmap2.at(BDF::C1)
            + cmap1.at(BDF::C2)*cmap2.at(BDF::PDF)
            + cmap2.at(BDF::C2)*cmap1.at(BDF::PDF);

        if(resbos -> GetProcess() -> GetInitState() == InitialState::gg)
            result += cmap1.at(BDF::G1)*cmap2.at(BDF::G1);

        return result;
    }

    // Scale Dependent contributions
    // alpha_s^1 Log^0 DeltaSigma function
    double DeltaSigma::V10log(const size_t&, const ConvMap& cmap1, const ConvMap& cmap2,
                              const double& /*lnMuR*/, const double &lnMuF) const {
        return 0.5*lnMuF*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)
                         +cmap2.at(BDF::P1)*cmap1.at(BDF::PDF));
    }

    // alpha_s^2 Log^2 DeltaSigma function
    double DeltaSigma::V22log(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2,
                              const double &lnMuR, const double &lnMuF) const {
        double A1 = GetA1();

        return lnMuR*0.5*beta0(nf)*A1*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
            - lnMuF*6.0/24.0*A1*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)
                                +cmap2.at(BDF::P1)*cmap1.at(BDF::PDF));
    }

    // alpha_s^2 Log^1 DeltaSigma function
    double DeltaSigma::V21log(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2,
                              const double &lnMuR, const double &lnMuF) const {
        double B1 = GetB1(nf);

        return lnMuR*beta0(nf)/2*(
                2*B1*cmap1.at(BDF::PDF)*cmap2.at(BDF::PDF)
                +cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)+cmap1.at(BDF::PDF)*cmap2.at(BDF::P1))
            - lnMuF/4*(2*B1*(cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)
                            +cmap1.at(BDF::PDF)*cmap2.at(BDF::P1))
                    + 2*cmap1.at(BDF::P1)*cmap2.at(BDF::P1)+cmap1.at(BDF::P1P1)*cmap2.at(BDF::PDF)
                    +cmap1.at(BDF::PDF)*cmap2.at(BDF::P1P1));
    }

    // alpha_s^2 Log^0 DeltaSigma function
    double DeltaSigma::V20log(const size_t& nf, const ConvMap& cmap1, const ConvMap& cmap2,
                              const double &lnMuR, const double &lnMuF) const {
        return -lnMuR*beta0(nf)*(cmap1.at(BDF::C1)*cmap2.at(BDF::PDF)
                                +cmap1.at(BDF::PDF)*cmap2.at(BDF::C1))
            + lnMuF/4*(2*cmap1.at(BDF::C1)*cmap2.at(BDF::P1)+2*cmap1.at(BDF::P1)*cmap2.at(BDF::C1)
                    + 2*cmap1.at(BDF::C1P1)*cmap2.at(BDF::PDF)
                    + 2*cmap1.at(BDF::PDF)*cmap2.at(BDF::C1P1)
                    + cmap1.at(BDF::P2)*cmap2.at(BDF::PDF)+cmap1.at(BDF::PDF)*cmap2.at(BDF::P2))
            - lnMuR*lnMuF/2*beta0(nf)*(
                    cmap1.at(BDF::P1)*cmap2.at(BDF::PDF)+cmap1.at(BDF::PDF)*cmap2.at(BDF::P1));
    }
}


