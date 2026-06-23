#include "gtest/gtest.h"
#include "ResBosIntegration.hh"

TEST_F(ResBosIntegration, Initialize) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));

    EXPECT_EQ(resbos -> GetECM(), 8000);
}

TEST_F(ResBosIntegration, Asym1) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("AsymOrder"))
        .WillOnce(Return(1));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
   
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        std::cout << resultCSS[i] << " " << resultCFG[i] << std::endl;
        EXPECT_LT(fabs(resultCSS[i] - resultCFG[i]),1E-10);
    }
}

TEST_F(ResBosIntegration, Asym2) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("AsymOrder"))
        .WillOnce(Return(2));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
   
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        std::cout << resultCSS[i] << " " << resultCFG[i] << std::endl;
        EXPECT_LT(fabs(resultCSS[i] - resultCFG[i]),1E-10);
    }
}

TEST_F(ResBosIntegration, Asym3) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("AsymOrder"))
        .WillOnce(Return(3));

    Asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
   
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        std::cout << resultCSS[i] << " " << resultCFG[i] << std::endl;
        EXPECT_LT(fabs(resultCSS[i] - resultCFG[i]),1E-10);
    }
}

TEST_F(ResBosIntegration, DeltaSigma0) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(0));

    Asym = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
    
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        std::cout << resultCSS[i] << " " << resultCFG[i] << std::endl;
        EXPECT_LT(fabs(resultCSS[i] - resultCFG[i]),1E-10);
    }
}

TEST_F(ResBosIntegration, DeltaSigma1) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(1));

    Asym = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
    
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        std::cout << resultCSS[i] << " " << resultCFG[i] << std::endl;
        EXPECT_LT(fabs(resultCSS[i] - resultCFG[i]),1E-10);
    }
}

TEST_F(ResBosIntegration, DeltaSigma2) {
    EXPECT_CALL(*settings,GetSettingDouble("muF"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings,GetSettingDouble("muR"))
        .WillOnce(Return(muF));
    EXPECT_CALL(*settings, GetSettingInt("DeltaSigmaOrder"))
        .WillOnce(Return(2));

    Asym = ResBos::CalculationFactory::Instance().Create("DeltaSigma");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
    
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        std::cout << resultCSS[i] << " " << resultCFG[i] << std::endl;
        EXPECT_LT(fabs(resultCSS[i] - resultCFG[i]),1E-10);
    }
}

TEST_F(ResBosIntegration, Resum2100) {
    EXPECT_CALL(*settings, GetSettingInt("AOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settings, GetSettingInt("BOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settings, GetSettingInt("COrder"))
        .WillOnce(Return(0));
    EXPECT_CALL(*settings, GetSettingInt("HOrder"))
        .WillOnce(Return(0));
    EXPECT_CALL(*settings,GetSettingDouble("C1"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("C2"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("C3"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("Q0"))
        .WillOnce(Return(1.55));
    EXPECT_CALL(*settings,GetSettingDouble("bMax"))
        .WillOnce(Return(1.5));
    EXPECT_CALL(*settings,GetSettingString("g"))
        .WillOnce(Return("0.181, 0.167, 0.003"));
    EXPECT_CALL(*settings,GetSettingString("NonPert"))
        .WillOnce(Return("SIYY"));

    Asym = ResBos::CalculationFactory::Instance().Create("Resummation");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
    resbos -> GetBeams().first -> GetConvolution() -> SetOrder(0);
    resbos -> GetBeams().second -> GetConvolution() -> SetOrder(0);
    
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        EXPECT_NEAR(resultCSS[i],resultCFG[i],pow(0.118,1)*(resultCFG[i]+resultCSS[i])/2.0);
    }
}

TEST_F(ResBosIntegration, Resum3211) {
    EXPECT_CALL(*settings, GetSettingInt("AOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settings, GetSettingInt("BOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settings, GetSettingInt("COrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settings, GetSettingInt("HOrder"))
        .WillOnce(Return(1));
    EXPECT_CALL(*settings,GetSettingDouble("C1"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("C2"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("C3"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("Q0"))
        .WillOnce(Return(1.55));
    EXPECT_CALL(*settings,GetSettingDouble("bMax"))
        .WillOnce(Return(1.5));
    EXPECT_CALL(*settings,GetSettingString("g"))
        .WillOnce(Return("0.181, 0.167, 0.003"));
    EXPECT_CALL(*settings,GetSettingString("NonPert"))
        .WillOnce(Return("SIYY"));

    Asym = ResBos::CalculationFactory::Instance().Create("Resummation");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
    resbos -> GetBeams().first -> GetConvolution() -> SetOrder(1);
    resbos -> GetBeams().second -> GetConvolution() -> SetOrder(1);
    
    resbos -> GetBeams().first -> GetConvolution() -> SetScheme(Scheme::CSS);
    resbos -> GetBeams().second -> GetConvolution() -> SetScheme(Scheme::CSS);
    resbos -> GetCalculation() -> SetScheme(Scheme::CSS);
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    resbos -> GetBeams().first -> GetConvolution() -> SetScheme(Scheme::CFG);
    resbos -> GetBeams().second -> GetConvolution() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        EXPECT_NEAR(resultCSS[i],resultCFG[i],pow(0.118,2)*(resultCSS[i]+resultCFG[i])/2.0);
    }
}

TEST_F(ResBosIntegration, Resum4322) {
    EXPECT_CALL(*settings, GetSettingInt("AOrder"))
        .WillOnce(Return(4));
    EXPECT_CALL(*settings, GetSettingInt("BOrder"))
        .WillOnce(Return(3));
    EXPECT_CALL(*settings, GetSettingInt("COrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settings, GetSettingInt("HOrder"))
        .WillOnce(Return(2));
    EXPECT_CALL(*settings,GetSettingDouble("C1"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("C2"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("C3"))
        .WillOnce(Return(1.0));
    EXPECT_CALL(*settings,GetSettingDouble("Q0"))
        .WillOnce(Return(1.55));
    EXPECT_CALL(*settings,GetSettingDouble("bMax"))
        .WillOnce(Return(1.5));
    EXPECT_CALL(*settings,GetSettingString("g"))
        .WillOnce(Return("0.181, 0.167, 0.003"));
    EXPECT_CALL(*settings,GetSettingString("NonPert"))
        .WillOnce(Return("SIYY"));

    Asym = ResBos::CalculationFactory::Instance().Create("Resummation");
    Asym -> Initialize(settings,resbos);

    resbos -> SetCalculation(std::move(Asym));
    resbos -> GetBeams().first -> GetConvolution() -> SetOrder(2);
    resbos -> GetBeams().second -> GetConvolution() -> SetOrder(2);

    resbos -> GetBeams().first -> GetConvolution() -> SetScheme(Scheme::CSS);
    resbos -> GetBeams().second -> GetConvolution() -> SetScheme(Scheme::CSS);
    resbos -> GetCalculation() -> SetScheme(Scheme::CSS);
    std::vector<double> resultCSS = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    resbos -> GetBeams().first -> GetConvolution() -> SetScheme(Scheme::CFG);
    resbos -> GetBeams().second -> GetConvolution() -> SetScheme(Scheme::CFG);
    resbos -> GetCalculation() -> SetScheme(Scheme::CFG);
    std::vector<double> resultCFG = resbos -> GetCalculation() -> GetCalc(Q,qt,y);

    for(size_t i = 0; i < resultCSS.size(); ++i) {
        EXPECT_NEAR(resultCSS[i],resultCFG[i],pow(0.118,3)*(resultCFG[i]+resultCSS[i])/2.0);
    }
}
