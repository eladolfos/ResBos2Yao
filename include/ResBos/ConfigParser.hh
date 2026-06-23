#ifndef CONFIGPARSER_HH
#define CONFIGPARSER_HH

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>
#include <fstream>
#include <algorithm>

struct ciLess {
    // Less than function to allow map to be independent of string capitalization
    bool operator()(const std::string &lhs, const std::string &rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

namespace IO {

    class Convert {
        public:
            //Convert primitive type T to a std::string
            template <typename T>
                static std::string TtoString(T const &val) {
                    std::ostringstream ostr;
                    ostr << val;

                    return ostr.str();
                }

            //Convert a std::string to primitive type T
            template <typename T>
                static T StringtoT(std::string const &val) {
                    std::istringstream istr(val);
                    T returnVal;
                    if(!(istr >> returnVal)) throw std::runtime_error("ConfigParser: Config File has an invalid type. Please check the Config File format");

                    return returnVal;
                }
    };

    template <> std::string Convert::StringtoT<std::string>(std::string const &val);
    template <> bool Convert::StringtoT<bool>(std::string const &val);

    class ConfigParser {
        // Configuration file parsing class
        public:
            ConfigParser() = default;
            ConfigParser(const std::string &fName);
            virtual ~ConfigParser() = default;
            virtual bool KeyExists(const std::string &key) const;
            template <typename ValueType> ValueType GetValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const {
                if(!KeyExists(key)) return defaultValue;

                return Convert::StringtoT<ValueType>(contents.find(key)->second);
            }


        private:
            std::map<std::string, std::string, ciLess> contents;
            std::string fName;
            inline void RemoveComment(std::string &line) const {
                if(line.find('#') != line.npos) line.erase(line.find('#'));
            };
            inline bool OnlyWhitespace(const std::string &line) const {
                return (line.find_first_not_of(" \t\n") == line.npos);
            };
            bool ValidLine(const std::string &line) const;
            void ExtractKey(std::string &key, size_t const &sepPos, const std::string &line) const;
            void ExtractValue(std::string &value, size_t const &sepPos, const std::string &line) const;
            void ExtractContents(const std::string &line);
            void ParseLine(const std::string &line, size_t const lineNo);
            void ExtractKeys();
    };

}

#endif
