#ifndef RESUMMATION_TEST_HH
#define RESUMMATION_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Resummation.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"
#include "../Beam/Beam_test.hh"
#include "../Process/Process_test.hh"

using ::testing::Return;
using ::testing::ReturnArg;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::An;
using ::testing::SaveArg;
using ::testing::DoAll;

class ResummationTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            settingsCSS = new IO::MockSettings;
            EXPECT_CALL(*settingsCSS,GetSettingDouble("C1"))
                .WillOnce(Return(1.0));
            EXPECT_CALL(*settingsCSS,GetSettingDouble("C2"))
                .WillOnce(Return(1.0));
            EXPECT_CALL(*settingsCSS,GetSettingDouble("C3"))
                .WillOnce(Return(1.0));
            EXPECT_CALL(*settingsCSS,GetSettingDouble("Q0"))
                .WillOnce(Return(1.55));
            EXPECT_CALL(*settingsCSS,GetSettingDouble("bMax"))
                .WillOnce(Return(1.5));
            EXPECT_CALL(*settingsCSS,GetSettingString("g"))
                .WillOnce(Return("0.181, 0.167, 0.003"));
            EXPECT_CALL(*settingsCSS,GetSettingString("NonPert"))
                .WillOnce(Return("SIYY"));
            EXPECT_CALL(*settingsCSS,GetSettingString("Scheme"))
                .WillOnce(Return("CSS"));
            EXPECT_CALL(*settingsCSS,GetSettingBool("KinematicCorrection"))
                .WillOnce(Return(false));

            settingsCFG = new IO::MockSettings;
            EXPECT_CALL(*settingsCFG,GetSettingDouble("C1"))
                .WillOnce(Return(1.0));
            EXPECT_CALL(*settingsCFG,GetSettingDouble("C2"))
                .WillOnce(Return(1.0));
            EXPECT_CALL(*settingsCFG,GetSettingDouble("C3"))
                .WillOnce(Return(1.0));
            EXPECT_CALL(*settingsCFG,GetSettingDouble("Q0"))
                .WillOnce(Return(1.55));
            EXPECT_CALL(*settingsCFG,GetSettingDouble("bMax"))
                .WillOnce(Return(1.5));
            EXPECT_CALL(*settingsCFG,GetSettingString("g"))
                .WillOnce(Return("0.181, 0.167, 0.003"));
            EXPECT_CALL(*settingsCFG,GetSettingString("NonPert"))
                .WillOnce(Return("SIYY"));
            EXPECT_CALL(*settingsCFG,GetSettingString("Scheme"))
                .WillOnce(Return("CFG"));
            EXPECT_CALL(*settingsCFG,GetSettingBool("KinematicCorrection"))
                .WillOnce(Return(false));

            beam = new MockBeam;

            processCSS = std::unique_ptr<MockProcess>(new MockProcess);
            EXPECT_CALL(*processCSS,GetChannels())
                .Times(AtLeast(1))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*processCSS,GetResumAngular())
                .Times(AtLeast(1))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*processCSS,GetCoupling(_))
                .WillRepeatedly(Return(0.5));
            EXPECT_CALL(*processCSS,GetPartons(0))
                .WillRepeatedly(Return(::ResBos::Partons({{1,-1},{3,-3},{5,-5}})));
            EXPECT_CALL(*processCSS,GetInitState())
                .WillRepeatedly(Return(InitialState::qq));
            EXPECT_CALL(*processCSS,CxFCxF(_,_,_,_))
                .WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*processCSS,H1())
                .WillRepeatedly(Return(4.0/3.0*(M_PI*M_PI-4)));
            EXPECT_CALL(*processCSS,H2(An<const size_t&>()))
                .WillRepeatedly(Return(19.98827381748085));
            EXPECT_CALL(*processCSS,H2(An<const double&>()))
                .WillRepeatedly(Return(19.98827381748085));
            EXPECT_CALL(*processCSS,GetAlphasCoupling())
                .WillRepeatedly(Return(0));

            resbosCSS = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbosCSS,GetProcess())
                .Times(AtLeast(4))
                .WillRepeatedly(Return(processCSS.get()));
            EXPECT_CALL(*resbosCSS,GetECM())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(8000));
            EXPECT_CALL(*resbosCSS,GetAlpi(_))
                .Times(AtLeast(2))
                .WillRepeatedly(Return(0.118/M_PI));
            EXPECT_CALL(*resbosCSS,GetNF(_))
                .WillRepeatedly(Return(5));
            EXPECT_CALL(*resbosCSS,H(0,_,_))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*resbosCSS,H(1,_,_))
                .WillRepeatedly(Return(1+0.118/M_PI*4.0/3.0*(M_PI*M_PI-4)));
            EXPECT_CALL(*resbosCSS,H(2,_,_))
                .WillRepeatedly(Return(1+0.118/M_PI*4.0/3.0*(M_PI*M_PI-4)+pow(0.118/M_PI,2)*18.5227));
            EXPECT_CALL(*resbosCSS,GetBeams())
                .WillRepeatedly(Return(std::make_pair(beam,beam)));

            processCFG = std::unique_ptr<MockProcess>(new MockProcess);
            EXPECT_CALL(*processCFG,GetChannels())
                .Times(AtLeast(1))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*processCFG,GetResumAngular())
                .Times(AtLeast(1))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*processCFG,GetCoupling(_))
                .WillRepeatedly(Return(0.5));
            EXPECT_CALL(*processCFG,GetPartons(0))
                .WillRepeatedly(Return(::ResBos::Partons({{1,-1},{3,-3},{5,-5}})));
            EXPECT_CALL(*processCFG,GetInitState())
                .WillRepeatedly(Return(InitialState::qq));
            EXPECT_CALL(*processCFG,CxFCxF(_,_,_,_))
                .WillRepeatedly(ReturnArg<0>());
            EXPECT_CALL(*processCFG,H1())
                .WillRepeatedly(Return(4.0/3.0*(M_PI*M_PI-4)));
            EXPECT_CALL(*processCFG,H2(An<const size_t&>()))
                .WillRepeatedly(Return(19.98827381748085));
            EXPECT_CALL(*processCFG,H2(An<const double&>()))
                .WillRepeatedly(Return(19.98827381748085));
            EXPECT_CALL(*processCFG,GetAlphasCoupling())
                .WillRepeatedly(Return(0));

            resbosCFG = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbosCFG,GetProcess())
                .Times(AtLeast(4))
                .WillRepeatedly(Return(processCFG.get()));
            EXPECT_CALL(*resbosCFG,GetECM())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(8000));
            EXPECT_CALL(*resbosCFG,GetAlpi(_))
                .Times(AtLeast(2))
                .WillRepeatedly(Return(0.118/M_PI));
            EXPECT_CALL(*resbosCFG,GetNF(_))
                .WillRepeatedly(Return(5));
            EXPECT_CALL(*resbosCFG,H(0,_,_))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*resbosCFG,H(1,_,_))
                .WillRepeatedly(Return(1+0.118/M_PI*4.0/3.0*(M_PI*M_PI-4)));
            EXPECT_CALL(*resbosCFG,H(2,_,_))
                .WillRepeatedly(Return(1+0.118/M_PI*4.0/3.0*(M_PI*M_PI-4)+pow(0.118/M_PI,2)*18.5227));
            EXPECT_CALL(*resbosCFG,GetBeams())
                .WillRepeatedly(Return(std::make_pair(beam,beam)));

        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete settingsCSS;
            delete settingsCFG;
            delete beam;
        }

        const double Q = 91.18, qt = 4.3035, y = 0; 
        std::unique_ptr<ResBos::Calculation> ResumCSS, ResumCFG;
        IO::MockSettings *settingsCSS;
        IO::MockSettings *settingsCFG;
        std::shared_ptr<MockResBos> resbosCSS, resbosCFG;
        std::unique_ptr<MockProcess> processCSS, processCFG;
        MockBeam* beam;
};

#endif

