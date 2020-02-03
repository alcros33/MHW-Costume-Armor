#pragma once
#include <map>
#include <string>
#include <iostream>

#include "easylogging++.h"

#ifdef NDEBUG
#define LOGGER_SETTINGS(NAME)                                           \
    logConfig.setGlobally(el::ConfigurationType::ToFile, "true");      \
    logConfig.setGlobally(el::ConfigurationType::Filename, NAME);     \
    logConfig.setGlobally(el::ConfigurationType::MaxLogFileSize, std::to_string(10*1024*1024) ) // 10MB
#else
#define LOGGER_SETTINGS(NAME) logConfig.setGlobally( el::ConfigurationType::ToStandardOutput, "true")
#endif

inline void INIT_LOGGER(const std::string &Name)
{
    el::Configurations logConfig;
    logConfig.setGlobally(el::ConfigurationType::Enabled, "true");
    logConfig.setGlobally(el::ConfigurationType::Format, "(%datetime) [%level] %msg");
    LOGGER_SETTINGS(Name);
    logConfig.setGlobally(el::ConfigurationType::LogFlushThreshold, "1");
    logConfig.setGlobally(el::ConfigurationType::SubsecondPrecision, "2");
    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", logConfig);
}

inline el::Level GLOBAL_LOG_LEVEL = el::Level::Debug;

// Hierarchy
inline std::map<el::Level, int> LOG_HIERARCHY{
    {el::Level::Info, 4},
    {el::Level::Fatal, 3},
    {el::Level::Error, 2},
    {el::Level::Warning, 1},
    {el::Level::Debug, 0},
};

// Names
inline std::map<std::string, el::Level> LOG_NAMES{
    {"Info", el::Level::Info},
    {"Fatal", el::Level::Fatal},
    {"Error", el::Level::Error},
    {"Warning", el::Level::Warning},
    {"Debug", el::Level::Debug},
};

// Capitalized
inline std::map<std::string, std::string> _capitalized{
    {"INFO","Info"},
    {"FATAL","Fatal"},
    {"ERROR","Error"},
    {"WARNING","Warning"},
    {"DEBUG", "Debug"}
};

inline bool check_level(std::string S)
{
    return LOG_HIERARCHY[LOG_NAMES[_capitalized[S]]] >= LOG_HIERARCHY[GLOBAL_LOG_LEVEL];
}

#define LOG_ENTRY(LEVEL,X) LOG_IF(check_level(#LEVEL), LEVEL) << X
#define LOG_ENTRY_HEX(LEVEL,X) LOG_IF(check_level(#LEVEL), LEVEL) << std::hex << X << std::dec

/// LOG Levels
// Global
// FATAL
// ERROR
// WARNING
// DEBUG