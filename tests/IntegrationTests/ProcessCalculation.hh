#ifndef PROCESSCALCULATION_HH
#define PROCESSCALCULATION_HH

#include "ResBos/ResBos.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Process.hh"
#include "../ResBos_test.hh"
#include "../Beam/Beam_test.hh"
#include "../Utilities/Settings_test.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

class ProcessCalculation : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            settings = new IO::MockSettings;
            // MockSettings for Calculation class
            EXPECT_CALL(*settings,GetSettingDouble("muF"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settings,GetSettingDouble("muR"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settings,GetSettingBool("KinematicCorrection"))
                .WillRepeatedly(Return(false));
            EXPECT_CALL(*settings,GetSettingString("scale"))
                .WillRepeatedly(Return("MT"));
            EXPECT_CALL(*settings,GetSettingString("Scheme"))
                .WillOnce(Return("CFG"));

            // MockSettings for Process class
            EXPECT_CALL(*settings,GetSettingString("EWMassScheme"))
                .Times(1)
                .WillRepeatedly(Return("ComplexMass"));
            EXPECT_CALL(*settings,GetSettingDouble("GF"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("Mw"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("Mz"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("gamW"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("gamZ"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("mT"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("mH"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("gamT"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingDouble("gamH"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(*settings,GetSettingInt("AlphaEMOrder"))
                .Times(1)
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*settings,GetSettingDouble("AlphaEM0"))
                .Times(1)
                .WillRepeatedly(Return(1.0/137.0));
            EXPECT_CALL(*settings,GetSettingDouble("AlphaEMMZ"))
                .Times(1)
                .WillRepeatedly(Return(1.0/127.0));
            EXPECT_CALL(*settings,GetSettingDouble("Qmin"))
                .Times(1)
                .WillRepeatedly(Return(66));
            EXPECT_CALL(*settings,GetSettingDouble("Qmax"))
                .Times(1)
                .WillRepeatedly(Return(116));
            EXPECT_CALL(*settings,GetSettingDouble("Qtmin"))
                .Times(1)
                .WillRepeatedly(Return(66));
            EXPECT_CALL(*settings,GetSettingDouble("Qtmax"))
                .Times(1)
                .WillRepeatedly(Return(116));
            EXPECT_CALL(*settings,GetSettingBool("KinematicCorrection"))
                .WillOnce(Return(false));

            // Mock Beam
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

            // Mock ResBos
            resbos = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbos,GetECM())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(8000));
            EXPECT_CALL(*resbos,GetAlpi(_))
                .Times(AtLeast(1))
                .WillRepeatedly(Return(0.118/M_PI));
            EXPECT_CALL(*resbos,GetNF(_))
                .WillRepeatedly(Return(5));
            EXPECT_CALL(*resbos,GetBeams())
                .WillRepeatedly(Return(std::make_pair(beam,beam)));

            // Process Setup
            process = ResBos::ProcessFactory::Instance().Create("DrellYan", resbos, *settings);
            process -> Initialize(*settings);

            EXPECT_CALL(*resbos,GetProcess())
                .Times(AtLeast(4))
                .WillRepeatedly(Return(process.get()));
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete settings;
            delete beam;
        }

        const double muF = 1.0, muR = 1.0;
        const double Q = 91.18, qt = 4.0, y = 0; 
        std::unique_ptr<ResBos::Calculation> Asym;
        IO::MockSettings *settings;
        std::shared_ptr<MockResBos> resbos;
        std::unique_ptr<ResBos::Process> process;
        MockBeam *beam;

};

#endif

