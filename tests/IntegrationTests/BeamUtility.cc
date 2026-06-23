#include "gtest/gtest.h"
#include "BeamUtility.hh"
#include "ResBos/Utility.hh"
#include "ResBos/ThreadPool.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::ReturnArg;
using ::testing::Invoke;

double C1CFGInt_q(double x) {
    return 1.0/6.0*(4-7*x+3*x*x+x*log(x));
}

double C1CFGInt_g(double x) {
    return -QCD::CF/2.0*x*log(x)*10;
}

double PlusFunc(double x) {
    return 1.0-x+x*log((1-x)/x);
}

double G1Int(double x) {
    return 4.0/3.0*(-1.0+1.0/(2.0*x)+0.5*x)*10+3.0*(-1.0+1.0/(2.0*x)+0.5*x);
}

const std::vector<double> C2qqMathematica = {1.012587854187173E8,
    1.012489249305247E7,1.011874493692432E6,
    100835.3905535625,9904.63119327473,
    914.190964552892,66.36621529449551,
    0.3219166671836787};

double C2CFGInt_q(double x) {
    using namespace QCD;
    return C2qqMathematica[static_cast<size_t>(8+static_cast<int>(log10(x)))] 
        + 0.5*((7.0*ZETA3/2.0-101.0/27.0)*CA*CF+14.0/27.0*CF*5)*PlusFunc(x);
}

const std::vector<double> C2ggMathematica = {4.230937896835231E8,
    4.232102920379431E7,4.239488720217416E6,
    428266.3445760571,45070.31479390583,
    5471.212160612752,830.17612917693,
    101.085049806342};

double C2CFGInt_g(double x) {
    using namespace QCD;
    return C2ggMathematica[static_cast<size_t>(8+static_cast<int>(log10(x)))]
        + ((-101.0/27.0+7.0/2.0*ZETA3)*CA*CA+14.0/27.0*CA*5)*PlusFunc(x);
}

TEST_F(BeamUtility, FFTSpeedC1) {
    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();

    Beam::Convolution conv(resbos,pdf,hoppet,false);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    conv.SetPDFs(pdfs);
    conv.GenerateGrid(Conv::C1);
}

TEST_F(BeamUtility, FFTSpeedC1P1) {
    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();

    Beam::Convolution conv(resbos,pdf,hoppet,false);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    conv.SetPDFs(pdfs);
    conv.GenerateGrid(Conv::C1P1);
}

TEST_F(BeamUtility, FFTSpeedC1P1P1) {
    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();

    Beam::Convolution conv(resbos,pdf,hoppet,false);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    conv.SetPDFs(pdfs);
    conv.GenerateGrid(Conv::C1P1P1);
}

TEST_F(BeamUtility, FFTSpeedC1P2) {
    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();

    Beam::Convolution conv(resbos,pdf,hoppet,false);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    conv.SetPDFs(pdfs);
    conv.GenerateGrid(Conv::C1P2);
}

TEST_F(BeamUtility, SpeedC2) {
    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();

    Beam::Convolution conv(resbos,pdf,hoppet,false);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    conv.SetPDFs(pdfs);
    conv.GenerateGrid(Conv::C2);
}

TEST_F(BeamUtility, SpeedC2P1) {
    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();

    Beam::Convolution conv(resbos,pdf,hoppet,false);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    conv.SetPDFs(pdfs);
    conv.GenerateGrid(Conv::C2P1);
}
