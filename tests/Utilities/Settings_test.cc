#include "Settings_test.hh"
#include "gtest/gtest.h"

TEST_F(SettingsTest,GetSetting) {
    IO::Settings settings;

    // Add dummy setting
    settings.AddSetting("itmax1","100");

    EXPECT_EQ(settings.GetSetting<int>("itmax1"), 100);

    EXPECT_EQ(settings.GetSetting<int>("aklsdghg"),0);
}

TEST_F(SettingsTest,CmdLinePrecedence) {
    IO::Settings settings;

    // Add dummy setting
    settings.AddSetting("itmax1","100");

    // Change setting using cmdLine
    settings.AddSettingCmdLine("itmax1","903475");

    EXPECT_EQ(settings.GetSetting<int>("itmax1"), 903475);
}

TEST_P(SettingsTest,CheckMandatory) {
    IO::Settings settings;

    EXPECT_THROW(settings.GetSetting<int>(GetParam()),std::runtime_error);

    settings.AddSettingCmdLine(GetParam(),"test");

    EXPECT_EQ(settings.GetSetting<std::string>(GetParam()),"test");
}

std::vector<std::string> mandatory{"AOrder","BOrder","COrder","HOrder","Process","PDF","NonPert","mode","XSec","scheme","PertOrder","AsymOrder"};

INSTANTIATE_TEST_SUITE_P(SettingsTestMandatory,SettingsTest,::testing::ValuesIn(mandatory));
