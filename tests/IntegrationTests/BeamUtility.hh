#ifndef BEAMUTILITY_HH
#define BEAMUTILITY_HH

#include "ResBos/ResBos.hh"
#include "ResBos/Beam.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Utility.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

class BeamUtility : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            IO::MockSettings *settings = new IO::MockSettings;
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

            pdf = new Utility::PDF(settings);
            hoppet = new Utility::Hoppet(pdf->GetPDF(),mc,mb,mt);

            delete settings;
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete pdf;
            delete hoppet;
        }

        const double muF = 1.0, muR = 1.0;
        const double Q = 91.18, qt = 4.0, y = 0; 
        const double mc = 1.3, mb = 4.75, mt = 173.5; 
        Utility::PDF *pdf;
        Utility::Hoppet *hoppet;

};

#endif

