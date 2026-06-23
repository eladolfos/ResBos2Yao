#ifndef LOG_HH
#define LOG_HH

#include <ostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <ctime>
#include <string>
#include <stdexcept>
#include <exception>
#include <memory>
#include <mutex>

enum class severityType {
    debug = 1,
    error,
    warning,
    info
};

class LogPolicyInterface {
    public:
        virtual void openStream(const std::string& name) = 0;
        virtual void closeStream() = 0;
        virtual void write(const std::string& msg) = 0;
};

class FileLogPolicy : public LogPolicyInterface {
    public:
        FileLogPolicy() : stream(new std::ofstream) {}
        virtual ~FileLogPolicy();
        void openStream(const std::string& name);
        void closeStream();
        void write(const std::string& msg);
    private:
        std::unique_ptr<std::ofstream> stream;
};

template<typename LogPolicy>
class Logger {
    public:
        Logger(const std::string& name);

        template<severityType severity, typename...Args>
        void Print(Args...args);

        ~Logger();
    private:
        unsigned logLineNumber;
        std::string GetTime();
        std::string GetLoglineHeader();
        std::stringstream logStream;
        LogPolicy* policy;
        std::mutex writeMutex;

        // Core printing functionality
        void printImpl();
        template<typename First, typename...Rest>
        void printImpl(First parm1, Rest...parm);
};

template<typename LogPolicy>
Logger<LogPolicy>::Logger(const std::string& name) {
    logLineNumber = 0;
    policy = new LogPolicy;
    if(!policy) throw std::runtime_error("LOGGER: Unable to create the logger instance");
    policy -> openStream(name);
}

template<typename LogPolicy>
Logger<LogPolicy>::~Logger() {
    if(policy) {
        policy -> closeStream();
        delete policy;
    }
}

template<typename LogPolicy>
template<severityType severity, typename...Args>
void Logger<LogPolicy>::Print(Args...args) {
    writeMutex.lock();
    switch(severity) {
        case severityType::debug:
            logStream << "<DEBUG>: ";
            break;
        case severityType::warning:
            logStream << "<WARNING>: ";
            break;
        case severityType::error:
            logStream << "<ERROR>: ";
            break;
        case severityType::info:
            logStream << "<INFO>: ";
            break;
    };
    printImpl(args...);
    writeMutex.unlock();
}

template<typename LogPolicy>
void Logger<LogPolicy>::printImpl() {
    policy -> write(GetLoglineHeader() + logStream.str());
    logStream.str("");
}

template<typename LogPolicy>
template<typename First, typename...Rest>
void Logger<LogPolicy>::printImpl(First parm1, Rest...parm) {
    logStream<<parm1;
    printImpl(parm...);
}

template<typename LogPolicy>
std::string Logger<LogPolicy>::GetTime() {
    std::string timeStr;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    timeStr = ctime(&in_time_t);
    //without the newline character
    return timeStr.substr(0,timeStr.size()-1);
}

template<typename LogPolicy>
std::string Logger<LogPolicy>::GetLoglineHeader() {
    std::stringstream header;
    header.str("");
    header.fill('0');
    header.width(3);
    header << logLineNumber++ << " <" << GetTime();
    header << "> ~ ";
    return header.str();
}

#endif
