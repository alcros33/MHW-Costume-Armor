#pragma once
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

#define DEBUG_LOG(LEVEL,X) LOG(LEVEL) << X
#define DEBUG_LOG_HEX(LEVEL,X) LOG(LEVEL) << std::hex << X << std::dec

/// LOG Levels
// FATAL
// ERROR
// WARNING
// DEBUG
// INFO