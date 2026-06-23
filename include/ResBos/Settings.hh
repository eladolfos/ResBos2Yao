#ifndef SETTINGS_HH
#define SETTINGS_HH

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "ResBos/ConfigParser.hh"
#include "ResBos/loguru.hpp"

namespace IO {

    class Settings {
        public:
            // Constructors and Destructors
            Settings() {
                cfgProcess = new ConfigParser();
                SetMandatory();
                SetDefaults();
            }
            Settings(const std::string&);
            virtual ~Settings();

            // Get Settings
            template<typename SettingsType> 
            SettingsType GetSetting(const std::string& key) const {
                if(cmdSettings.find(key) != cmdSettings.end()) 
                    return Convert::StringtoT<SettingsType>(cmdSettings.find(key) -> second);
                else if(cfgProcess->KeyExists(key)) return cfgProcess->GetValueOfKey<SettingsType>(key); 
                else if(defaultSettings.find(key) != defaultSettings.end())
                    return Convert::StringtoT<SettingsType>(defaultSettings.find(key)->second);
                else if(std::find(mandatorySettings.begin(), mandatorySettings.end(), key) != mandatorySettings.end())
                    throw std::runtime_error("Settings: Mandatory option for " + key + " does not exist. Please check your configuration files!");
                else {
                    LOG_F(WARNING,"Setting: %s not found. This may effect results. Setting to 0.", key.c_str());
                    return 0;
                }
            }

            virtual std::string GetSettingString(const std::string& key) const;

            virtual double GetSettingDouble(const std::string& key) const {
                return GetSetting<double>(key);
            }

            virtual int GetSettingInt(const std::string& key) const {
                return GetSetting<int>(key);
            }

            virtual size_t GetSettingSizet(const std::string& key) const {
                return GetSetting<size_t>(key);
            }

            virtual bool GetSettingBool(const std::string& key) const;

            virtual void AddSetting(std::string,std::string);
            virtual void AddSettingCmdLine(std::string,std::string);

            // Ensure the setting exists
            bool KeyExists(const std::string &key) const {
                if(cfgProcess->KeyExists(key)) return true; 
                else return false;
            }
        private:
            std::vector<std::string> mandatorySettings;
            std::map<std::string, std::string, ciLess> defaultSettings; 
            std::map<std::string, std::string, ciLess> cmdSettings; 
            void SetDefaults();
            void SetMandatory();
            std::string fName;
            ConfigParser *cfgProcess;
    };

    template<> 
    std::string Settings::GetSetting(const std::string& key) const;

}

#endif
