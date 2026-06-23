#ifndef PDF_TEST_HH
#define PDF_TEST_HH

#include "gmock/gmock.h"
#include "../Utilities/Settings_test.hh"
#include "ResBos/Pdf.hh"

using ::testing::Return;
using ::testing::_;

class MockPDF : public Utility::PDF {
    public:
        double Alphas(const double&) { return 0.118; }
        double Alpi(const double&) { return 0.118/M_PI; }
        double Apdf(const int& pid, const double& x, const double&) {
            if(pid != 0) 
                return x;
            return x;
        }
        size_t NF(const double&) { return 5; }
        double qMin() const { return 1.3; }
};

class PdfTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            IO::MockSettings settings;

            EXPECT_CALL(settings,GetSettingString("PDF"))
                .WillOnce(Return("CT14nnlo"));

            EXPECT_CALL(settings,GetSettingInt("iSet"))
                .WillOnce(Return(0));

            EXPECT_CALL(settings,GetSettingDouble("mz"))
                .WillOnce(Return(91.18))
                .WillOnce(Return(91.18));

            EXPECT_CALL(settings,GetSettingDouble("md"))
                .WillOnce(Return(0.0));

            EXPECT_CALL(settings,GetSettingDouble("mu"))
                .WillOnce(Return(0.0));

            EXPECT_CALL(settings,GetSettingDouble("ms"))
                .WillOnce(Return(0.0));

            EXPECT_CALL(settings,GetSettingDouble("mc"))
                .WillOnce(Return(1.3));

            EXPECT_CALL(settings,GetSettingDouble("mb"))
                .WillOnce(Return(4.75));

            EXPECT_CALL(settings,GetSettingDouble("mt"))
                .WillOnce(Return(172));

            pdf = new Utility::PDF(&settings);
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete pdf; 
        }

        Utility::PDF* pdf;
};

#endif

