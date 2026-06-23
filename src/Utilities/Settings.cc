#include "ResBos/Settings.hh"

namespace IO {

    Settings::Settings(const std::string &fName_){
        cfgProcess = new ConfigParser(fName_);     
        SetMandatory();
        SetDefaults();
    }

    Settings::~Settings() {
        delete cfgProcess;
    }

    void Settings::SetMandatory() {
        mandatorySettings = {
            "AOrder",
            "BOrder",
            "COrder",
            "HOrder",
            "Process",
            "PDF",
            "NonPert",
            "mode",
            "XSec",
            "PertOrder",
            "AsymOrder",
            "scheme"
        };
    }

    void Settings::SetDefaults() {
        //Default Settings
        defaultSettings["EWMassScheme"] = "OnShell";
        defaultSettings["beam"] = "pp"; 
        defaultSettings["ECM"] = "13000"; 
        defaultSettings["bMax"] = "1.5"; 
        defaultSettings["Q0"] = "1.5"; 
        defaultSettings["CFlags"] = "1";
        defaultSettings["ngag"] = "1";
        defaultSettings["mz"] = "91.18";
        defaultSettings["mw"] = "80.48";
        defaultSettings["mh"] = "125";
        defaultSettings["mt"] = "172";
        defaultSettings["mb"] = "4.75";
        defaultSettings["mc"] = "1.3";
        defaultSettings["ms"] = "0";
        defaultSettings["md"] = "0";
        defaultSettings["mu"] = "0";
        
        // Default Vegas Settings
        defaultSettings["evaluations"] = "1000";
        defaultSettings["maxinc"] = "1000";
        defaultSettings["nCubeBatch"] = "1000";
        defaultSettings["maxCube"] = "1e9";
        defaultSettings["maxEvalCube"] = "1e7";
        defaultSettings["iterations"] = "10";
        defaultSettings["alpha"] = "1.5";
        defaultSettings["beta"] = "0.75";
        defaultSettings["adapt"] = "true";
        defaultSettings["adaptErrors"] = "false";
        defaultSettings["vegas_rtol"] = "0";
        defaultSettings["vegas_atol"] = "0";
        defaultSettings["sync"] = "true";
    }

    void Settings::AddSetting(std::string name, std::string value) {
        defaultSettings[name] = value;
    }

    void Settings::AddSettingCmdLine(std::string name, std::string value) {
        cmdSettings[name] = value;
    }

    template<> 
    std::string Settings::GetSetting(const std::string& key) const {
        if(cmdSettings.find(key) != cmdSettings.end()) 
            return cmdSettings.find(key) -> second;
        else if(cfgProcess->KeyExists(key)) return cfgProcess->GetValueOfKey<std::string>(key); 
        else if(std::find(mandatorySettings.begin(), mandatorySettings.end(), key) == mandatorySettings.end()) 
            return defaultSettings.find(key)->second;
        else if(defaultSettings.find(key) != defaultSettings.end())
            return defaultSettings.find(key) -> second;
        else throw std::runtime_error("Settings: Option for " + key + " does not exist. Please check your configuration files!");
    }

    template<> 
    bool Settings::GetSetting(const std::string& key) const {
        std::string tmp = GetSetting<std::string>(key);
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        if(tmp == "true") return true;
        else if(tmp == "false") return false;
        else throw std::runtime_error("Settings:: Option " + key + " is supposed to be true or false. Please check your configuration files!");
    }


    std::string Settings::GetSettingString(const std::string& key) const {
        return GetSetting<std::string>(key);
    }

    bool Settings::GetSettingBool(const std::string& key) const {
        return GetSetting<bool>(key);
    }

}
