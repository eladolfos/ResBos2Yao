#include "gtest/gtest.h"
#include "Pdf_test.hh"
#include "ResBos/DoubleExponential.hh"

TEST_F(PdfTest, Constructors) {
    IO::MockSettings settings;

    EXPECT_CALL(settings,GetSettingDouble("mz"))
        .Times(2)
        .WillRepeatedly(Return(91.18));

    EXPECT_CALL(settings,GetSettingDouble("md"))
        .WillOnce(Return(0.0));

    EXPECT_CALL(settings,GetSettingDouble("mu"))
        .WillOnce(Return(0.0));

    EXPECT_CALL(settings,GetSettingDouble("ms"))
        .WillOnce(Return(0.0));

    EXPECT_CALL(settings,GetSettingDouble("mc"))
        .WillOnce(Return(1.3));

    EXPECT_CALL(settings,GetSettingDouble("mb"))
        .WillOnce(Return(4.75));

    EXPECT_CALL(settings,GetSettingDouble("mt"))
        .WillOnce(Return(172));

    Utility::PDF pdf2("CT14nnlo",0,&settings);

    EXPECT_EQ(pdf -> Apdf(1,0.1,91.18), pdf2.Apdf(1,0.1,91.18));
    EXPECT_EQ(pdf -> Alphas(200), pdf2.Alphas(200));
}

TEST_F(PdfTest, Alphas) {
    EXPECT_LT(fabs(pdf -> Alphas(91.18) - 0.118), 1E-3);
}

TEST_F(PdfTest, ValenceCheck) {
    std::function<double(double)> func1 = [this](double x) {
        return (this -> pdf -> Apdf(1,x,91.18) - this -> pdf -> Apdf(-1,x,91.18));
    };

    std::function<double(double)> func2 = [this](double x) {
        return (this -> pdf -> Apdf(2,x,91.18) - this -> pdf -> Apdf(-2,x,91.18));
    };

    std::function<double(double)> func3 = [this](double x) {
        return (this -> pdf -> Apdf(3,x,91.18) - this -> pdf -> Apdf(-3,x,91.18));
    };

    std::function<double(double)> func4 = [this](double x) {
        return (this -> pdf -> Apdf(4,x,91.18) - this -> pdf -> Apdf(-4,x,91.18));
    };

    std::function<double(double)> func5 = [this](double x) {
        return (this -> pdf -> Apdf(5,x,91.18) - this -> pdf -> Apdf(-5,x,91.18));
    };

    Utility::DoubleExponential integral1(func1);
    Utility::DoubleExponential integral2(func2);
    Utility::DoubleExponential integral3(func3);
    Utility::DoubleExponential integral4(func4);
    Utility::DoubleExponential integral5(func5);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double dVal = 0, uVal = 0, sVal = 0, cVal = 0, bVal = 0;

    double rerr = 1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        dVal += integral1.Integrate(xBins[i],xBins[i+1],rerr,aerr);
        uVal += integral2.Integrate(xBins[i],xBins[i+1],rerr,aerr);
        sVal += integral3.Integrate(xBins[i],xBins[i+1],rerr,aerr);
        cVal += integral4.Integrate(xBins[i],xBins[i+1],rerr,aerr);
        bVal += integral5.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }
    
    EXPECT_LT(fabs(dVal-1),1E-3);
    EXPECT_LT(fabs(uVal-2),1E-3);
    EXPECT_LT(fabs(sVal),1E-3);
    EXPECT_LT(fabs(cVal),1E-3);
    EXPECT_LT(fabs(bVal),1E-3);
}

TEST_F(PdfTest, MomentumSum) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> pdf -> Apdf(i,x,91.18);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr = 1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum-1),1E-4);
}

TEST_F(PdfTest, NF) {
    EXPECT_EQ(pdf -> NF(1.2),3);
    EXPECT_EQ(pdf -> NF(2),4);
    EXPECT_EQ(pdf -> NF(123),5);
    EXPECT_EQ(pdf -> NF(12395),5);
}
