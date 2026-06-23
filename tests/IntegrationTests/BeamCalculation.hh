#ifndef BEAMCALCULATION_HH
#define BEAMCALCULATION_HH

#include "ResBos/ResBos.hh"
#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/HoppetInterface.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"
#include "../Process/Process_test.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::An;

class BeamCalculation : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            settings = new IO::MockSettings;
            EXPECT_CALL(*settings,GetSettingDouble("muF"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settings,GetSettingDouble("muR"))
                .WillOnce(Return(muF));
            EXPECT_CALL(*settings,GetSettingBool("KinematicCorrection"))
                .WillOnce(Return(false));
            EXPECT_CALL(*settings,GetSettingString("scale"))
                .WillRepeatedly(Return("MT"));
            EXPECT_CALL(*settings,GetSettingString("Scheme"))
                .WillOnce(Return("CFG"));
            EXPECT_CALL(*settings,GetSettingString("PDF"))
                .WillOnce(Return("CT14nnlo"));
            EXPECT_CALL(*settings,GetSettingInt("iSet"))
                .WillOnce(Return(0));
            EXPECT_CALL(*settings,GetSettingDouble("mz"))
                .WillOnce(Return(91.18))
                .WillOnce(Return(91.18));
            EXPECT_CALL(*settings,GetSettingDouble("md"))
                .WillOnce(Return(0.0));
            EXPECT_CALL(*settings,GetSettingDouble("mu"))
                .WillOnce(Return(0.0));
            EXPECT_CALL(*settings,GetSettingDouble("ms"))
                .WillOnce(Return(0.0));
            EXPECT_CALL(*settings,GetSettingDouble("mc"))
                .WillOnce(Return(1.3));
            EXPECT_CALL(*settings,GetSettingDouble("mb"))
                .WillOnce(Return(4.75));
            EXPECT_CALL(*settings,GetSettingDouble("mt"))
                .WillOnce(Return(172));

            process = std::unique_ptr<MockProcess>(new MockProcess);
            EXPECT_CALL(*process,GetChannels())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(1));
            EXPECT_CALL(*process,GetResumAngular())
                .WillRepeatedly(Return(2));
            EXPECT_CALL(*process,GetCoupling(_))
                .WillRepeatedly(Return(0.5));
            EXPECT_CALL(*process,GetPartons(0))
                .WillRepeatedly(Return(::ResBos::Partons({{1,-1},{3,-3},{5,-5}})));
            EXPECT_CALL(*process,GetInitState())
                .WillRepeatedly(Return(InitialState::qq));
            EXPECT_CALL(*process,H1())
                .WillRepeatedly(Return(4.0/3.0*(M_PI*M_PI-4)));
            EXPECT_CALL(*process,H2(An<const size_t&>()))
                .WillRepeatedly(Return(19.98827381748085));
            EXPECT_CALL(*process,H2(An<const double&>()))
                .WillRepeatedly(Return(19.98827381748085));

            resbos = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbos,GetProcess())
                .Times(AtLeast(4))
                .WillRepeatedly(Return(process.get()));
            EXPECT_CALL(*resbos,GetAlpi(_))
                .Times(AtLeast(1))
                .WillRepeatedly(Return(0.118/M_PI));
            EXPECT_CALL(*resbos,GetNF(_))
                .WillRepeatedly(Return(5));

            Utility::PDF *pdf = new Utility::PDF(settings);
            Utility::Hoppet *hoppet = new Utility::Hoppet(pdf->GetPDF(),mc,mb,mt);
            Beam::Convolution *conv = new Beam::Convolution(resbos,pdf,hoppet,false);
            
            std::map<std::thread::id,Utility::PDF*> pdfs;
            pdfs[std::this_thread::get_id()] = pdf;
            conv -> SetPDFs(pdfs);

            beams = Beam::Beam::MakeBeams("pp",8000);
            beams.first -> SetPDF(pdf);
            beams.first -> SetHoppet(hoppet);
            beams.first -> SetConvolution(conv);
            beams.second -> SetPDF(pdf);
            beams.second -> SetHoppet(hoppet);
            beams.second -> SetConvolution(conv);

            EXPECT_CALL(*resbos,GetBeams())
                .WillRepeatedly(Return(beams));
            EXPECT_CALL(*resbos,GetECM())
                .Times(AtLeast(2))
                .WillRepeatedly(Return(beams.first -> Energy()));
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete settings;
            delete beams.first;
        }

        const double muF = 1.0, muR = 1.0;
        const double Q = 91.18, qt = 2.0, y = 0; 
        const double mc = 1.3, mb = 4.75, mt = 173.5; 
        std::unique_ptr<ResBos::Calculation> Asym;
        IO::MockSettings *settings;
        std::shared_ptr<MockResBos> resbos;
        std::unique_ptr<MockProcess> process;
        std::pair<Beam::Beam*,Beam::Beam*> beams;

};

#endif
