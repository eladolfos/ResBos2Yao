#ifndef RESBOSINTEGRATION_HH
#define RESBOSINTEGRATION_HH

#include "ResBos/ResBos.hh"
#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"
#include "ResBos/HoppetInterface.hh"
#include "../Utilities/Settings_test.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

class ResBosIntegration : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            settings = new IO::MockSettings;
            resbos = std::make_shared<ResBos::ResBos>();

            // Settings to setup Calculation class
            EXPECT_CALL(*settings,GetSettingString("Scheme"))
                .WillOnce(Return("CSS"));

            // Settings to setup Beam class
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

            // Beam setup
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
           
            resbos -> SetBeams(beams);

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

            // Process setup
            process = ResBos::ProcessFactory::Instance().Create("DrellYan", resbos, *settings);
            process -> Initialize(*settings);

            resbos -> SetProcess(std::move(process));
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete settings;
        }

        const double muF = 1.0, muR = 1.0;
        const double Q = 91.18, qt = 4.0, y = 0; 
        const double mc = 1.3, mb = 4.75, mt = 173.5; 
        std::unique_ptr<ResBos::Calculation> Asym;
        IO::MockSettings *settings;
        std::shared_ptr<ResBos::ResBos> resbos;
        std::unique_ptr<ResBos::Process> process;
        std::pair<Beam::Beam*,Beam::Beam*> beams;

};

#endif

