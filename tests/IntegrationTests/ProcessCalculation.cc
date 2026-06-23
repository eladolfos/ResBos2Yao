#include "gtest/gtest.h"
#include "ProcessCalculation.hh"

TEST_F(ProcessCalculation, Asym1) {
    EXPECT_CALL(*settings,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    std::vector<double> results = Asym -> GetCalc(Q,qt,y);

    EXPECT_GT(results[0], 0);
    EXPECT_LT(results[1]/results[0], 1E-16);
}

TEST_F(ProcessCalculation, Asym2) {
    EXPECT_CALL(*settings,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    std::vector<double> results = Asym -> GetCalc(Q,qt,y);

    EXPECT_GT(results[0], 0);
    EXPECT_LT(results[1]/results[0], 1E-15);
}

TEST_F(ProcessCalculation, Asym3) {
    EXPECT_CALL(*settings,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    std::vector<double> results = Asym -> GetCalc(Q,qt,y);

    EXPECT_GT(results[0], 0);
    EXPECT_LT(results[1]/results[0], 1E-15);
}

TEST_F(ProcessCalculation, DeltaSigma0) {
    EXPECT_CALL(*settings,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(0));

    Asym = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    Asym -> Initialize(settings,resbos);

    std::vector<double> results = Asym -> GetCalc(Q,qt,y);

    EXPECT_GT(results[0], 0);
    EXPECT_LT(results[1]/results[0], 1E-16);
}

TEST_F(ProcessCalculation, DeltaSigma1) {
    EXPECT_CALL(*settings,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(1));

    Asym = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    Asym -> Initialize(settings,resbos);

    std::vector<double> results = Asym -> GetCalc(Q,qt,y);

    EXPECT_GT(results[0], 0);
    EXPECT_LT(results[1]/results[0], 1E-16);
}

TEST_F(ProcessCalculation, DeltaSigma2) {
    EXPECT_CALL(*settings,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(2));

    Asym = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    Asym -> Initialize(settings,resbos);

    std::vector<double> results = Asym -> GetCalc(Q,qt,y);

    EXPECT_GT(results[0], 0);
    EXPECT_LT(results[1]/results[0], 1E-16);
}

