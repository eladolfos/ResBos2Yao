#include "gtest/gtest.h"
#include "DeltaSigma_test.hh"

TEST_F(DeltaSigmaTest, V0) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(0));
    EXPECT_CALL(*settingsCFG,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(0));

    DeltaSigmaCSS = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    DeltaSigmaCFG = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    DeltaSigmaCSS -> Initialize(settingsCSS,resbos);
    DeltaSigmaCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = DeltaSigmaCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = DeltaSigmaCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(DeltaSigmaTest, V1) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCFG,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(1));

    DeltaSigmaCSS = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    DeltaSigmaCFG = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    DeltaSigmaCSS -> Initialize(settingsCSS,resbos);
    DeltaSigmaCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = DeltaSigmaCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = DeltaSigmaCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(DeltaSigmaTest, V2) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(2));

    DeltaSigmaCSS = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    DeltaSigmaCFG = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    DeltaSigmaCSS -> Initialize(settingsCSS,resbos);
    DeltaSigmaCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = DeltaSigmaCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = DeltaSigmaCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}
