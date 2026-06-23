#ifndef HASH_TEST_HH
#define HASH_TEST_HH

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Settings_test.hh"

using ::testing::Return;

class HashTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            settings = std::make_shared<MockSettings>(); 
            ON_CALL(*settings, GetSettingString("beam"))
                .WillByDefault(Return("pp"));
            ON_CALL(*settings, GetSettingString("ECM"))
                .WillByDefault(Return("13000"));
            ON_CALL(*settings, GetSettingString("Process"))
                .WillByDefault(Return("DrellYan"));
            ON_CALL(*settings, GetSettingString("PDF"))
                .WillByDefault(Return("CT14nnlo"));
            ON_CALL(*settings, GetSettingString("iSet"))
                .WillByDefault(Return("0"));
            ON_CALL(*settings, GetSettingString("KinematicCorrection"))
                .WillByDefault(Return("false"));
        }

        // Clean-up work for each test
        virtual void TearDown() {

        }

        std::shared_ptr<MockSettings> settings;
};

#endif // ifndef HASH_TEST_HH
