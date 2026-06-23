#ifndef ASYMPTOTIC_TEST_HH
#define ASYMPTOTIC_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Asymptotic.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"
#include "../Beam/Beam_test.hh"
#include "../Process/Process_test.hh"

using ::testing::Return;
using ::testing::ReturnPointee;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::An;

class AsymptoticTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            settingsCSS = new IO::MockSettings;
            EXPECT_CALL(*settingsCSS,GetSettingDouble("muF"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settingsCSS,GetSettingDouble("muR"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settingsCSS,GetSettingString("Scheme"))
                .WillOnce(Return("CSS"));
            EXPECT_CALL(*settingsCSS,GetSettingBool("KinematicCorrection"))
                .WillOnce(Return(false));

            settingsCFG = new IO::MockSettings;
            EXPECT_CALL(*settingsCFG,GetSettingDouble("muF"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settingsCFG,GetSettingDouble("muR"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settingsCFG,GetSettingString("Scheme"))
                .WillOnce(Return("CFG"));
            EXPECT_CALL(*settingsCFG,GetSettingBool("KinematicCorrection"))
                .WillOnce(Return(false));

            process = std::unique_ptr<MockProcess>(new MockProcess);
            EXPECT_CALL(*process,GetChannels())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*process,GetResumAngular())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(2));
            EXPECT_CALL(*process,GetCoupling(_))
                .WillRepeatedly(Return(0.5));
            EXPECT_CALL(*process,GetPartons(0))
                .WillRepeatedly(Return(ResBos::Partons({{1,-1},{3,-3},{5,-5}})));
            EXPECT_CALL(*process,H1())
                .WillRepeatedly(Return(4.0/3.0*(M_PI*M_PI-4)));
            EXPECT_CALL(*process,H2(An<const size_t&>()))
                .WillRepeatedly(Return(19.98827381748085));
            EXPECT_CALL(*process,H2(An<const double&>()))
                .WillRepeatedly(Return(19.98827381748085));

            double x = Q/8000.0;
            beam = new MockBeam;
            EXPECT_CALL(*beam,BeamMap(_,_,_,_))
                .Times(AtLeast(2))
                .WillRepeatedly(Return(std::map<::Beam::BDF,double>({
                            {::Beam::BDF::PDF,x},
                            {::Beam::BDF::P1,x},
                            {::Beam::BDF::P2,x},
                            {::Beam::BDF::P1P1,x},
                            {::Beam::BDF::C1,x},
                            {::Beam::BDF::C1P1,x},
                            {::Beam::BDF::P3,x},
                            {::Beam::BDF::P1P2,x},
                            {::Beam::BDF::P2P1,x},
                            {::Beam::BDF::P1P1P1,x},
                            {::Beam::BDF::C1P2,x},
                            {::Beam::BDF::C1P1P1,x},
                            {::Beam::BDF::C2,x},
                            {::Beam::BDF::C2P1,x}
                            })));

            resbos = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbos,GetProcess())
                .Times(AtLeast(4))
                .WillRepeatedly(Return(process.get()));
            EXPECT_CALL(*resbos,GetECM())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(8000));
            EXPECT_CALL(*resbos,GetAlpi(_))
                .Times(AtLeast(2))
                .WillRepeatedly(Return(0.118/M_PI));
            EXPECT_CALL(*resbos,GetNF(_))
                .WillRepeatedly(Return(5));
            EXPECT_CALL(*resbos,GetBeams())
                .WillRepeatedly(Return(std::make_pair(beam,beam)));
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete settingsCSS;
            delete settingsCFG;
            delete beam;
        }

        const double muF = 1.0, muR = 1.0;
        const double Q = 91.18, qt = 55.3035, y = 0; 
        std::unique_ptr<ResBos::Calculation> AsymCSS, AsymCFG;
        IO::MockSettings *settingsCSS;
        IO::MockSettings *settingsCFG;
        std::shared_ptr<MockResBos> resbos;
        std::unique_ptr<MockProcess> process;
        MockBeam *beam;
};

#endif
