#include "ResBos/Settings.hh"
#include "ResBos/Electroweak.hh"

#include "ResBos/loguru.hpp"

Electroweak::Electroweak(const IO::Settings *s){ 
    // Setup how the vector boson masses are to be handled
    std::string schemeStr = s->GetSettingString("EWMassScheme");
    LOG_S(INFO) << "Electroweak Scheme: " << schemeStr;
    if(schemeStr.compare("OnShell") == 0) massScheme = EWMassScheme::OnShell;
    else if(schemeStr.compare("PoleScheme") == 0) massScheme = EWMassScheme::PoleScheme;
    else if(schemeStr.compare("ComplexMass") == 0) massScheme = EWMassScheme::ComplexMass;
    else throw std::runtime_error("Electroweak: Invalid mass scheme choice. Check manual for valid options.");

    // Setup what the EW input parameters
//    bool flagGF = s.KeyExisits("GF");
//    bool flagA0 = s.KeyExisits("alpha0");
//    bool flagAMZ = s.KeyExisits("alphaMz");
//    bool flagMZ = s.KeyExisits("Mz");
//    bool flagMW = s.KeyExisits("Mw");
//    bool flagSW = s.KeyExisits("sw");
//    std::cout << flagGF << flagA0 << flagAMZ << flagMZ << flagMW << flagSW << std::endl;
//    if(flagGF && flagMW && flagMZ) {inputs=EWInputs::GFMWMZ; std::cout << "1" << std::endl;}
//    else if(flagA0 && flagMW && flagMZ) {inputs=EWInputs::AMWMZ;  std::cout << "2" << std::endl;}
//    else if(flagA0 && flagSW && flagMZ) {inputs=EWInputs::ASWMZ;  std::cout << "3" << std::endl;}
//    else if(flagA0 && flagSW && flagGF) {inputs=EWInputs::ASWGF;  std::cout << "4" << std::endl;}
//    else if(flagA0 && flagGF && flagMZ) {inputs=EWInputs::AGFMZ;  std::cout << "5" << std::endl;}

//    fixedAlpha = s.GetSetting<bool>("FixedAlpha");

    inputs = EWInputs::GFMWMZ;
    switch(inputs) {
        case(EWInputs::GFMWMZ):
            GF = s->GetSettingDouble("GF");
            mW = s->GetSettingDouble("Mw");
            mZ = s->GetSettingDouble("Mz");
            gamW = s->GetSettingDouble("gamW");
            gamZ = s->GetSettingDouble("gamZ");
            break;
        case(EWInputs::AMWMZ):
            alpha0 = s->GetSettingDouble("alpha0");
            mW = s->GetSettingDouble("Mw");
            mZ = s->GetSettingDouble("Mz");
            gamW = s->GetSettingDouble("gamW");
            gamZ = s->GetSettingDouble("gamZ");
            break;
        case(EWInputs::ASWMZ):
            alpha0 = s->GetSettingDouble("alpha0");
            sw = s->GetSettingDouble("sw");
            mZ = s->GetSettingDouble("Mz");
            gamZ = s->GetSettingDouble("gamZ");
            break;
        case(EWInputs::ASWGF):
            alpha0 = s->GetSettingDouble("alpha0");
            sw = s->GetSettingDouble("sw");
            GF = s->GetSettingDouble("GF");
            break;
        case(EWInputs::AGFMZ):
            alpha0 = s->GetSettingDouble("alpha0");
            GF = s->GetSettingDouble("GF");
            mZ = s->GetSettingDouble("Mz");
            gamZ = s->GetSettingDouble("gamZ");
            break;
        default:
            throw std::runtime_error("The current requested scheme is not implemented yet");
    }

    mT = s->GetSettingDouble("mT");
    mH = s->GetSettingDouble("mH");
    gamT = s->GetSettingDouble("gamT");
    gamH = s->GetSettingDouble("gamH");

    if(massScheme==EWMassScheme::ComplexMass) {
        mZ.imag(gamZ);
        mW.imag(gamW);
        mH.imag(gamH);
        mT.imag(gamT);
    }

    LOG_F(INFO,"Calculating Electroweak parameters");
    CalculateParams();
    LOG_F(INFO,"W Mass = %f + %f i",mW.real(),mW.imag());
    LOG_F(INFO,"W Width = %f",gamW);
    LOG_F(INFO,"Z Mass = %f + %f i",mZ.real(),mZ.imag());
    LOG_F(INFO,"Z Width = %f",gamZ);
    LOG_F(INFO,"H Mass = %f + %f i",mH.real(),mH.imag());
    LOG_F(INFO,"H Width = %f",gamH);
    LOG_F(INFO,"top Mass = %f + %f i",mT.real(),mT.imag());
    LOG_F(INFO,"top Width = %f",gamT);
    LOG_F(INFO,"GF = %f",GF);
    LOG_F(INFO,"alpha0 = %f + %f i",alpha0.real(),alpha0.imag());
    LOG_F(INFO,"sin^2 \\theta_w = %f + %f i",(sws).real(),(sws).imag());
    LOG_F(INFO,"Photon Coupling = %f + %f i",acoupl.real(),acoupl.imag());
    LOG_F(INFO,"Z Coupling = %f + %f i",zcoupl.real(),zcoupl.imag());
    LOG_F(INFO,"W Coupling = %f +%f i",wcoupl.real(),wcoupl.imag());

    // Setup CKM Matrix
    const double km_l = 0.224;
    const double km_a = 0.83;
    const double km_eta = 0.35;
    const double VUB = 35.7E-4;
    const double km_rho = sqrt(pow(VUB/km_a/pow(km_l,3),2)-pow(km_eta,2));

    CKM[2] = pow(1.0-km_l*km_l/2.0-pow(km_l,4)/8.0,2);                          // |V_ud|^2 
    CKM[6] = pow(km_l,2);                                                       // |V_us|^2
    CKM[10] = pow(km_a*pow(km_l,3),2)*(pow(km_rho,2)+pow(km_eta,2));            // |V_ub|^2

    CKM[4] = pow(-km_l+0.5*pow(km_a,2)*pow(km_l,5)
            *sqrt(pow(1.0-2.0*km_rho,2)+km_eta*km_eta),2);                      // |V_cd|^2 
    CKM[12] = pow(1.0-km_l*km_l/2.0-pow(km_l,4)/8.0*(1.0+4*pow(km_a,2)),2);     // |V_cs|^2
    CKM[20] = pow(km_a*km_l*km_l,2);                                            // |V_cb|^2

    // Initialize Alpha_EM
    initAlpha(s);
}
        
void Electroweak::CalculateParams() {
    if(inputs != EWInputs::GFMWMZ) 
        throw std::runtime_error("Electroweak: Currently those inputs are not implemented!");

//    if(massScheme==EWMassScheme::PoleScheme) 
//        throw std::runtime_error("Electroweak: Currently the Pole Mass Scheme is not implemented!");

    cws = mW*mW/mZ/mZ;
    sws = static_cast<std::complex<double>>(1) - cws;

    cw = sqrt(cws);
    sw = sqrt(sws);
    
    alpha0 = sqrt(2.0)*mW*mW*GF*sws/M_PI;
    acoupl = 2.0*M_PI*alpha0;
    zcoupl = M_PI*alpha0/sws/cws;
    wcoupl = sqrt(2.0)*mW*mW*GF/2.0;
}

void Electroweak::initAlpha(const IO::Settings *settings) {
    // Read in order and values at 0 and m_Z
    alphaEMOrder = settings -> GetSettingInt("AlphaEMOrder");
    alphaEM0 = settings -> GetSettingDouble("AlphaEM0");
    alphaEMMz = settings -> GetSettingDouble("AlphaEMMZ");

    // Get alpha_EM at matching scales and matching beta values
    if(alphaEMOrder <= 0) return;

    // Evolve from mZ to tau/charm
    alphaEMStep[4] = alphaEMMz / (1. + alphaEMMz * betaEM[4] * log(std::abs(mZ*mZ)/Q2Step[4]));
    alphaEMStep[3] = alphaEMStep[4] / (1. - alphaEMStep[4] * betaEM[3] 
                   * log(Q2Step[3]/Q2Step[4]));

    // Evolve up from me to light-quark threshold
    alphaEMStep[0] = alphaEM0;
    alphaEMStep[1] = alphaEMStep[0]/(1. - alphaEMStep[0] * betaEM[0] * log(Q2Step[1]/Q2Step[0]));
    alphaEMStep[2] = alphaEMStep[1]/(1. - alphaEMStep[1] * betaEM[1] * log(Q2Step[2]/Q2Step[1]));

    // Interpolate beta between light-quark and tau/charm region
    betaEM[2] = (1./alphaEMStep[3] - 1./alphaEMStep[2])/log(Q2Step[2]/Q2Step[3]);
}

void Electroweak::MCFMInit(double mzIn, double mwIn, double GFin) {

    GF = GFin;
    mW = mwIn;
    mZ = mzIn;

    std::complex<double> cws = mW*mW/mZ/mZ;
    sws = static_cast<std::complex<double>>(1) - cws;

    cw = sqrt(cws);
    sw = sqrt(sws);

    alpha0 = sqrt(2.0)*mW*mW*GF*sws/M_PI;
    acoupl = 2.0*M_PI*alpha0;
    zcoupl = M_PI*alpha0/sws/cws;
    wcoupl = sqrt(2.0)*mW*mW*GF/2.0;

    // Read in order and values at 0 and m_Z
    alphaEMOrder = 1;
    alphaEM0 = alpha0.real();
    alphaEMMz = 0.0078125;

    // Get alpha_EM at matching scales and matching beta values
    if(alphaEMOrder <= 0) return;

    // Evolve from mZ to tau/charm
    alphaEMStep[4] = alphaEMMz / (1. + alphaEMMz * betaEM[4] * log(std::abs(mZ*mZ)/Q2Step[4]));
    alphaEMStep[3] = alphaEMMz / (1. - alphaEMStep[4] * betaEM[3] * log(Q2Step[3]/Q2Step[4]));

    // Evolve up from me to light-quark threshold
    alphaEMStep[0] = alphaEM0;
    alphaEMStep[1] = alphaEMStep[0]/(1. - alphaEMStep[0] * betaEM[0] * log(Q2Step[1]/Q2Step[0]));
    alphaEMStep[2] = alphaEMStep[1]/(1. - alphaEMStep[1] * betaEM[1] * log(Q2Step[2]/Q2Step[1]));

    // Interpolate beta between light-quark and tau/charm region
    betaEM[2] = (1./alphaEMStep[3] - 1./alphaEMStep[2])/log(Q2Step[2]/Q2Step[3]);
}

double Electroweak::GetAlphaEM(double Q2) const {
    // Fixed alpha_EM
    if(alphaEMOrder == 0) return alphaEM0;
    if(alphaEMOrder < 0) return alphaEMMz;

    // Running alpha_EM
    for(size_t i = 4; i != static_cast<size_t>(-1); --i)
        if(Q2 > Q2Step[i])
            return alphaEMStep[i] / (1. - betaEM[i] * alphaEMStep[i] * log(Q2/Q2Step[i]));

    return alphaEM0;
}
