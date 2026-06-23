#ifndef ELECTROWEAK_TEST_HH
#define ELECTROWEAK_TEST_HH

#include "gmock/gmock.h"
#include "Settings_test.hh"
#include "ResBos/Electroweak.hh"

using ::testing::Return;
using ::testing::_;

class MockElectroweak : public Electroweak {
    public:
        virtual ~MockElectroweak() {}

        // Mock Getter functions for couplings
        MOCK_CONST_METHOD0(GetSW2, std::complex<double>());
        MOCK_CONST_METHOD0(GetZCoupl, std::complex<double>());
        MOCK_CONST_METHOD0(GetACoupl, std::complex<double>());

        // Mock Getter functions Z boson
        MOCK_CONST_METHOD0(GetMzComplex, std::complex<double>());
        MOCK_CONST_METHOD0(GetMzReal, double());
        MOCK_CONST_METHOD0(GetGamZ, double());

        // Mock Getter functions W boson
        MOCK_CONST_METHOD0(GetMwComplex, std::complex<double>());
        MOCK_CONST_METHOD0(GetMwReal, double());
        MOCK_CONST_METHOD0(GetGamW, double());

        // Mock Getter functions H boson
        MOCK_CONST_METHOD0(GetMhComplex, std::complex<double>());
        MOCK_CONST_METHOD0(GetMhReal, double());
        MOCK_CONST_METHOD0(GetGamH, double());

        // Mock Getter functions t quark
        MOCK_CONST_METHOD0(GetMtComplex, std::complex<double>());
        MOCK_CONST_METHOD0(GetMtReal, double());
        MOCK_CONST_METHOD0(GetGamT, double());
};

class ElectroweakTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            IO::MockSettings settings;

            EXPECT_CALL(settings,GetSettingString("EWMassScheme"))
                .WillOnce(Return("OnShell"))
                .WillOnce(Return("PoleScheme"))
                .WillOnce(Return("ComplexMass"));

            EXPECT_CALL(settings,GetSettingDouble("GF"))
                .Times(3)
                .WillRepeatedly(Return(GF));

            EXPECT_CALL(settings,GetSettingDouble("Mw"))
                .Times(3)
                .WillRepeatedly(Return(Mw));

            EXPECT_CALL(settings,GetSettingDouble("Mz"))
                .Times(3)
                .WillRepeatedly(Return(Mz));

            EXPECT_CALL(settings,GetSettingDouble("gamW"))
                .Times(3)
                .WillRepeatedly(Return(gamW));

            EXPECT_CALL(settings,GetSettingDouble("gamZ"))
                .Times(3)
                .WillRepeatedly(Return(gamZ));

            EXPECT_CALL(settings,GetSettingDouble("mT"))
                .Times(3)
                .WillRepeatedly(Return(mT));

            EXPECT_CALL(settings,GetSettingDouble("mH"))
                .Times(3)
                .WillRepeatedly(Return(mH));

            EXPECT_CALL(settings,GetSettingDouble("gamT"))
                .Times(3)
                .WillRepeatedly(Return(gamT));

            EXPECT_CALL(settings,GetSettingDouble("gamH"))
                .Times(3)
                .WillRepeatedly(Return(gamH));

            EXPECT_CALL(settings,GetSettingInt("AlphaEMOrder"))
                .Times(3)
                .WillRepeatedly(Return(1));

            EXPECT_CALL(settings,GetSettingDouble("AlphaEM0"))
                .Times(3)
                .WillRepeatedly(Return(1.0/137.0));

            EXPECT_CALL(settings,GetSettingDouble("AlphaEMMZ"))
                .Times(3)
                .WillRepeatedly(Return(1.0/127.0));

            ewOnShell = new Electroweak(&settings);
            ewPole = new Electroweak(&settings);
            ewComplex = new Electroweak(&settings);
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete ewOnShell; 
            delete ewPole; 
            delete ewComplex;
        }

        const double Mw = 80.358;
        const double Mz = 91.118;
        const double gamW = 2.035;
        const double gamZ = 2.50;
        const double mT = 172;
        const double mH = 125;
        const double gamT = 0;
        const double gamH = 0.004;
        const double GF = 1.1663787E-5;
        Electroweak *ewOnShell, *ewPole, *ewComplex;
};


#endif
