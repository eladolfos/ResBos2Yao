#include "gtest/gtest.h"
#include "Resummation_test.hh"

ACTION_P(CFGModify1, param) {
    return arg0 - 0.5*(19.98827381748085)*param*0.118;
}

TEST_F(ResummationTest, W210) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingInt("BOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCSS,GetSettingInt("COrder"))
        .WillOnce(Return(0));
    EXPECT_CALL(*settingsCSS,GetSettingInt("HOrder"))
        .WillOnce(Return(0));

    EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    EXPECT_CALL(*settingsCFG,GetSettingInt("AOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("BOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCFG,GetSettingInt("COrder"))
        .WillOnce(Return(0));
    EXPECT_CALL(*settingsCFG,GetSettingInt("HOrder"))
        .WillOnce(Return(0));

    EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    ResumCSS = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCFG = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCSS -> Initialize(settingsCSS,resbosCSS);
    ResumCFG -> Initialize(settingsCFG,resbosCFG);

    std::vector<double> resultsCSS = ResumCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = ResumCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-7);
    }
}

// Need to figure out how to get CxFCxF to give different results for CSS and CFG to account for the H factor
TEST_F(ResummationTest, W321) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingInt("BOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingInt("COrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCSS,GetSettingInt("HOrder"))
        .WillOnce(Return(1));

    EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    EXPECT_CALL(*settingsCFG,GetSettingInt("AOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingInt("BOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("COrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settingsCFG,GetSettingInt("HOrder"))
        .WillOnce(Return(1));

    EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
        .WillRepeatedly(CFGModify1(1));

    ResumCSS = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCFG = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCSS -> Initialize(settingsCSS,resbosCSS);
    ResumCFG -> Initialize(settingsCFG,resbosCFG);

    std::vector<double> resultsCSS = ResumCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = ResumCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_GT(resultsCSS[i],0);
        EXPECT_GT(resultsCFG[i],0);
    }
}

TEST_F(ResummationTest, W432) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settingsCSS,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    EXPECT_CALL(*settingsCFG,GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settingsCFG,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
        .WillRepeatedly(CFGModify1(1));

    ResumCSS = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCFG = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCSS -> Initialize(settingsCSS,resbosCSS);
    ResumCFG -> Initialize(settingsCFG,resbosCFG);

    std::vector<double> resultsCSS = ResumCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = ResumCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_GT(resultsCSS[i],0);
        EXPECT_GT(resultsCFG[i],0);
    }
}

TEST_F(ResummationTest, W532_central) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AOrder"))
        .WillOnce(Return(5));
    EXPECT_CALL(*settingsCSS,GetSettingString("A5Error"))
        .WillOnce(Return("Central"));
    EXPECT_CALL(*settingsCSS,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    EXPECT_CALL(*settingsCFG,GetSettingInt("AOrder"))
        .WillOnce(Return(5));
    EXPECT_CALL(*settingsCFG,GetSettingString("A5Error"))
        .WillOnce(Return("Central"));
    EXPECT_CALL(*settingsCFG,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
        .WillRepeatedly(CFGModify1(1));

    ResumCSS = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCFG = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCSS -> Initialize(settingsCSS,resbosCSS);
    ResumCFG -> Initialize(settingsCFG,resbosCFG);

    std::vector<double> resultsCSS = ResumCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = ResumCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_GT(resultsCSS[i],0);
        EXPECT_GT(resultsCFG[i],0);
    }
}

TEST_F(ResummationTest, SchemeChangeCFG2CSS) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settingsCSS,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    EXPECT_CALL(*settingsCFG,GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settingsCFG,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    ResumCSS = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCFG = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCSS -> Initialize(settingsCSS,resbosCSS);
    ResumCFG -> Initialize(settingsCFG,resbosCFG);
    ResumCFG -> SetScheme(Scheme::CSS);

    std::vector<double> resultsCSS = ResumCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = ResumCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-7);
    }
}

TEST_F(ResummationTest, SchemeChangeCSS2CFG) {
    EXPECT_CALL(*settingsCSS,GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settingsCSS,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCSS,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCSS,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    EXPECT_CALL(*settingsCFG,GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settingsCFG,GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settingsCFG,GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settingsCFG,GetSettingInt("HOrder"))
        .WillOnce(Return(2));

    EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
        .WillRepeatedly(ReturnArg<0>());

    ResumCSS = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCFG = ResBos::CalculationFactory::Instance().Create("Resummation");
    ResumCSS -> Initialize(settingsCSS,resbosCSS);
    ResumCFG -> Initialize(settingsCFG,resbosCFG);
    ResumCSS -> SetScheme(Scheme::CFG);

    std::vector<double> resultsCSS = ResumCSS -> GetCalc(Q,qt,y);
    std::vector<double> resultsCFG = ResumCFG -> GetCalc(Q,qt,y);

    for(unsigned int i = 0; i < resultsCSS.size(); ++i) {
        EXPECT_LT(fabs(resultsCSS[i] - resultsCFG[i]), 1E-7);
    }
}
