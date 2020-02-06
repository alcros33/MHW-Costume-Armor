#pragma once
#include <map>
#include <string>
#include <iostream>
#include <QString>

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
    logConfig.setGlobally(el::ConfigurationType::Format, "%datetime [%level] %msg");
    LOGGER_SETTINGS(Name);
    logConfig.setGlobally(el::ConfigurationType::LogFlushThreshold, "1");
    logConfig.setGlobally(el::ConfigurationType::SubsecondPrecision, "1");
    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", logConfig);
}

inline QString GLOBAL_LOG_LEVEL = "DEBUG";

// Hierarchy
inline std::map<QString, el::Level> LOG_HIERARCHY{
    {"INFO", el::Level::Info},
    {"FATAL", el::Level::Fatal},
    {"ERROR", el::Level::Error},
    {"WARNING", el::Level::Warning},
    {"DEBUG", el::Level::Debug},
};

inline bool check_level(QString S)
{
    return LOG_HIERARCHY[S] >= LOG_HIERARCHY[GLOBAL_LOG_LEVEL];
}

#define LOG_ENTRY(LEVEL,X) LOG_IF(check_level(#LEVEL), LEVEL) << X
#define LOG_ENTRY_HEX(LEVEL,X) LOG_IF(check_level(#LEVEL), LEVEL) << std::hex << X << std::dec

/// LOG Levels
// Global
// FATAL
// ERROR
// WARNING
// DEBUG