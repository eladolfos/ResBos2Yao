#include "gtest/gtest.h"
#include "Electroweak_test.hh"

TEST_F(ElectroweakTest, Couplings) {
    // Check sin(theta_w)^2
    EXPECT_DOUBLE_EQ(std::abs(ewOnShell->GetSW2()),0.2222323395619455);
    EXPECT_DOUBLE_EQ(std::abs(ewPole->GetSW2()),0.2222323395619455);
    EXPECT_DOUBLE_EQ(std::abs(ewComplex->GetSW2()),0.2223501575660604);

    // Check alpha0
    EXPECT_DOUBLE_EQ(std::abs(ewOnShell->GetAlpha0()),0.0075347735060878095);
    EXPECT_DOUBLE_EQ(std::abs(ewPole->GetAlpha0()),0.0075347735060878095);
    EXPECT_DOUBLE_EQ(std::abs(ewComplex->GetAlpha0()),0.007543602834179484);

    // Check acoupl
    EXPECT_DOUBLE_EQ(std::abs(ewOnShell->GetACoupl()),0.047342378186376939);
    EXPECT_DOUBLE_EQ(std::abs(ewPole->GetACoupl()),0.047342378186376939);
    EXPECT_DOUBLE_EQ(std::abs(fabs(ewComplex->GetACoupl())),0.0473978544909148);

    // Check zcoupl
    EXPECT_DOUBLE_EQ(std::abs(ewOnShell->GetZCoupl()),0.1369502833513832);
    EXPECT_DOUBLE_EQ(std::abs(ewPole->GetZCoupl()),0.1369502833513832);
    EXPECT_DOUBLE_EQ(std::abs(fabs(ewComplex->GetZCoupl())),0.1370533776374083);

    // Check wcoupl
    EXPECT_DOUBLE_EQ(std::abs(ewOnShell->GetWCoupl()),0.05325775073926699);
    EXPECT_DOUBLE_EQ(std::abs(ewPole->GetWCoupl()),0.05325775073926699);
    EXPECT_DOUBLE_EQ(std::abs(ewComplex->GetWCoupl()),0.05329190567003856);
}

TEST_F(ElectroweakTest, CKM) {
    double CKM11 = ewOnShell -> GetCKM(1,2);
    double CKM12 = ewOnShell -> GetCKM(1,4);
    double CKM21 = ewOnShell -> GetCKM(3,2);
    double CKM22 = ewOnShell -> GetCKM(3,4);
    double CKM31 = ewOnShell -> GetCKM(5,2);
    double CKM32 = ewOnShell -> GetCKM(5,4);

    // Check first column unitarity
    EXPECT_NEAR(1.0,CKM11+CKM21+CKM31,3E-5);

    // Check second column unitarity
    EXPECT_NEAR(1.0,CKM12+CKM22+CKM32,3E-5);
}
