#include "ResBos/ConfigParser.hh"
#include "ResBos/loguru.hpp"

namespace IO {

    // Templated function to convert a string to another format
    template <> std::string Convert::StringtoT<std::string>(std::string const &val) {
        return val;
    }

    // Templated function to convert a string to a bool
    template <> bool Convert::StringtoT<bool>(std::string const &val) {
        std::string input = val;
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        if(val=="true") return true;
        else if(val=="false") return false;
        else throw std::runtime_error("ConfigParser: Config File has an invalid type. Please check the Config File format");
    }

    ConfigParser::ConfigParser(const std::string &fName_) : fName(fName_) {
        // Class initializer for configuration file parser
        ExtractKeys();
    }

    bool ConfigParser::KeyExists(const std::string &key) const {
        // Function to test to see if a given setting exists in the parser
        return contents.find(key) != contents.end();
    }

    bool ConfigParser::ValidLine(const std::string &line) const {
        // Function to test to see if the current line being read is a valid configuration line
        std::string temp = line;
        temp.erase(0, temp.find_first_not_of("\t "));
        if(temp[0] == '=') return false;

        for(size_t i = temp.find('=') + 1; i < temp.length(); i++) {
            if(temp[i] != ' ') return true;
        }

        return false;
    }

    void ConfigParser::ExtractKey(std::string &key, size_t const &sepPos, const std::string &line) const {
        // Extract the name of the setting to be stored in the map
        key = line.substr(0, sepPos);
        if(key.find('\t') != line.npos || key.find(' ') != line.npos) key.erase(key.find_first_of("\t "));
    }

    void ConfigParser::ExtractValue(std::string &value, size_t const &sepPos, const std::string &line) const {
        // Extract the value for a given setting to be stored in the map
        value = line.substr(sepPos +1);
        value.erase(0,value.find_first_not_of("\t "));
        value.erase(value.find_last_not_of("\t ") + 1);
    }

    void ConfigParser::ExtractContents(const std::string &line) {
        // Extract the settings from a given configuration file
        std::string temp = line;
        temp.erase(0, temp.find_first_not_of("\t "));
        size_t sepPos = temp.find('=');

        std::string key, value;
        ExtractKey(key, sepPos, temp);
        ExtractValue(value, sepPos, temp);

        if(!KeyExists(key)) {
            DLOG_S(INFO) << "Found option (" << key << ":" << value << ")";
            contents.insert(std::pair<std::string, std::string>(key, value));
        } else
            throw std::runtime_error("ConfigParser: Repeated options in " + fName + ". Please check!");
    }

    void ConfigParser::ParseLine(const std::string &line, size_t const lineNo) {
        // Function to parse each line in the configuration file and ensure proper formatting
        if(line.find('=') == line.npos)
            throw std::runtime_error("ConfigParser: Couldn't find separator on line: " + Convert::TtoString(lineNo));

        if(!ValidLine(line))
            throw std::runtime_error("ConfigParser: Bad format for line: " + Convert::TtoString(lineNo));

        ExtractContents(line);
    }

    void ConfigParser::ExtractKeys() {
        // Extracts all keys from the configuration file
        std::ifstream file;
        file.open(fName.c_str());
        if(!file) throw std::runtime_error("ConfigParser: File " + fName + " couldn't be opened. Check to make sure it exists");

        std::string line;
        size_t lineNo = 0;
        while(std::getline(file, line)) {
            lineNo++;
            std::string temp = line;

            if(temp.empty()) continue;

            RemoveComment(temp);
            if(OnlyWhitespace(temp)) continue;

            ParseLine(temp, lineNo);
        }

        file.close();
    }

}
