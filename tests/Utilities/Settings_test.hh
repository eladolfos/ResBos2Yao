#ifndef SETTINGS_TEST_HH
#define SETTINGS_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Settings.hh"

namespace IO {

class MockSettings : public Settings {
    public:
        // Mock getter functions
        MOCK_CONST_METHOD1(GetSettingString,std::string(const std::string&));
        MOCK_CONST_METHOD1(GetSettingDouble,double(const std::string&));
        MOCK_CONST_METHOD1(GetSettingInt,int(const std::string&));
        MOCK_CONST_METHOD1(GetSettingBool,bool(const std::string&));
};

}

class SettingsTest : public ::testing::TestWithParam<std::string> {
    protected:
        // Set-up work for each test
        virtual void SetUp() {}

        // Clean-up work for each test
        virtual void TearDown() {}
};
        

#endif
