#include "gtest/gtest.h"
#include "Asymptotic_test.hh"
#include "../Utilities/Electroweak_test.hh"

TEST_F(AsymptoticTest, C1) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::qq));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C2) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::qq));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C3) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::qq));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C1_MT) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::qq));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C2_MT) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::qq));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C3_MT) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"))
        .WillOnce(Return("MT"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::qq));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C1_gg) {
    std::unique_ptr<MockElectroweak> EW = std::unique_ptr<MockElectroweak>(new MockElectroweak);
    EXPECT_CALL(*EW,GetMtReal())
        .WillRepeatedly(Return(172.5));
    EXPECT_CALL(*EW,GetMhReal())
        .WillRepeatedly(Return(125));

    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::gg));
    EXPECT_CALL(*process,GetEW())
        .WillRepeatedly(Return(EW.get()));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C2_gg) {
    std::unique_ptr<MockElectroweak> EW = std::unique_ptr<MockElectroweak>(new MockElectroweak);
    EXPECT_CALL(*EW,GetMtReal())
        .WillRepeatedly(Return(172.5));
    EXPECT_CALL(*EW,GetMhReal())
        .WillRepeatedly(Return(125));

    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::gg));
    EXPECT_CALL(*process,GetEW())
        .WillRepeatedly(Return(EW.get()));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}

TEST_F(AsymptoticTest, C3_gg) {
    std::unique_ptr<MockElectroweak> EW = std::unique_ptr<MockElectroweak>(new MockElectroweak);
    EXPECT_CALL(*EW,GetMtReal())
        .WillRepeatedly(Return(172.5));
    EXPECT_CALL(*EW,GetMhReal())
        .WillRepeatedly(Return(125));

    EXPECT_CALL(*settingsCSS,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingString("scale"))
        .WillOnce(Return("Q"))
        .WillOnce(Return("Q"));
    EXPECT_CALL(*process,GetInitState())
        .WillRepeatedly(Return(InitialState::gg));
    EXPECT_CALL(*process,GetEW())
        .WillRepeatedly(Return(EW.get()));

    AsymCSS = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCFG = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    AsymCSS -> Initialize(settingsCSS,resbos);
    AsymCFG -> Initialize(settingsCFG,resbos);

    std::vector<double> resultsCSS = AsymCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = AsymCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-8);
    }
}
