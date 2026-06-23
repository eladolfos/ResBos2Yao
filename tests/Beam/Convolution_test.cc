#include "gtest/gtest.h"
#include "Convolution_test.hh"
#include "ResBos/Utility.hh"
#include "ResBos/ThreadPool.hh"

#include <memory>

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

const std::vector<double> C2qqMathematica = {
    1.012489249305247E7,3.201210456691356E6,
    1.011874493692438E6,319650.0217618,
    100835.3905535550,31709.52247476577,
    9904.63119328367,3050.986652392331,
    914.190964555004,259.6626722025456,
    66.36621529445643,13.21816012089143,
    0.3219166671836644,-2.442490773971518
};

double C2CFGInt_q(double x) {
    using namespace QCD;
    return C2qqMathematica[static_cast<size_t>(14+static_cast<int>(2*log10(x)))] 
        + 0.5*((7.0*ZETA3/2.0-101.0/27.0)*CA*CF+14.0/27.0*CF*5)*PlusFunc(x);
}

const std::vector<double> C2ggMathematica = {
    4.232102920379411E7,1.338978889701513E7,
    4.239488720217416E6,1.344693625316311E6,
    428266.3445760636,137638.1882705652,
    45070.31479381482,15280.8694249541,
    5471.212160604897,2088.807602398201,
    830.176129177062,317.7912598824772,
    101.085049806342,22.53580737854334
};

double C2CFGInt_g(double x) {
    using namespace QCD;
    return C2ggMathematica[static_cast<size_t>(14+static_cast<int>(2*log10(x)))]
        + ((-101.0/27.0+7.0/2.0*ZETA3)*CA*CA+14.0/27.0*CA*5)*PlusFunc(x);
}

std::map<int,double> MakeMap(double x, double, int, int, int) {
    std::map<int,double> map({
            {-5,x},
            {-4,x},
            {-3,x},
            {-2,x},
            {-1,x},
            {0,x},
            {1,x},
            {2,x},
            {3,x},
            {4,x},
            {5,x}
            });

    return map;
}

TEST_P(ConvolutionTest, GetConvC) {
    double x = pow(10,GetParam());

    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();
    MockPDF pdf;
    MockHoppet hoppet;

    Beam::Convolution conv(resbos,&pdf,&hoppet,false);

    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = &pdf;
    conv.SetPDFs(pdfs);

    EXPECT_NEAR(conv.GetConv(1,x,91.18,Conv::C),
                pdf.Apdf(1,x,91.18)
                    +pdf.Alpi(91.18)*(C1CFGInt_q(x)+pdf.Alpi(91.18)*(C2CFGInt_q(x))),
                1E-8);
}


TEST_P(ConvolutionTest, GetConvC1) {
    double x = pow(10,GetParam());

    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();
    MockPDF pdf;
    MockHoppet hoppet;

    Beam::Convolution conv(resbos,&pdf,&hoppet,false);

    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = &pdf;
    conv.SetPDFs(pdfs);

    EXPECT_NEAR(conv.GetConv(1,x,91.18,Conv::C1),C1CFGInt_q(x), 1E-8);

    EXPECT_NEAR(conv.GetConv(0,x,91.18,Conv::C1),C1CFGInt_g(x), 1E-8);
}

TEST_P(ConvolutionTest, GetConvC2CFG) {
    double x = pow(10,GetParam());

    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();
    MockPDF pdf;
    MockHoppet hoppet;

    Beam::Convolution conv(resbos,&pdf,&hoppet,false);

    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = &pdf;
    conv.SetPDFs(pdfs);

    EXPECT_NEAR(conv.GetConv(1,x,91.18,Conv::C2),C2CFGInt_q(x), 1E-8);

    EXPECT_NEAR(conv.GetConv(0,x,91.18,Conv::C2),C2CFGInt_g(x), 1E-8);
}

TEST_P(ConvolutionTest, GetConvG1) {
    double x = pow(10,GetParam());

    std::shared_ptr<MockResBos> resbos = std::make_shared<MockResBos>();
    MockPDF pdf;
    MockHoppet hoppet;

    Beam::Convolution conv(resbos,&pdf,&hoppet,false);

    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = &pdf;
    conv.SetPDFs(pdfs);

    EXPECT_NEAR(conv.GetConv(0,x,91.18,Conv::G1),G1Int(x), 1E-8);
}

INSTANTIATE_TEST_SUITE_P(ConvolutionTestXCheck,ConvolutionTest,::testing::Range(-7.0,0.0,0.5));
