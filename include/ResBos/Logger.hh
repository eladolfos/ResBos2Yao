#ifndef LOGGER_HH
#define LOGGER_HH

// Logger Class taken from Dr. Dobbs:
// www.drdobbs.com/cpp/a-lightweight-logger-for-c/240147595?pgno

#include "ResBos/Log.hh"

static Logger<FileLogPolicy> LogInst("resbos.log");

#define LOG LogInst.Print<severityType::info>
#define LOG_ERR LogInst.Print<severityType::error>
#define LOG_WARN LogInst.Print<severityType::warning>

#ifdef LOGGING_LEVEL_1

#define LOG_DEBUG LogInst.Print<severityType::debug>

#else

#define LOG_DEBUG(...)

#endif

#ifdef LOGGING_LEVEL_2

#define ELOG_DEBUG LogInst.Print<severityType::debug>
#define ELOG_ERR LogInst.Print<severityType::error>
#define ELOG_WARN LogInst.Print<severityType::warning>

#else

#define ELOG_DEBUG(...)
#define ELOG_ERR(...)
#define ELOG_WARN(...)

#endif

#endif
