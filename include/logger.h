#ifndef CAFMAKER_LOGGER_H
#define CAFMAKER_LOGGER_H

#include <iostream>
#include <string>
#include <cstdlib>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    CRITICAL = 3
};

inline LogLevel& get_global_log_level() {
    static LogLevel level = LogLevel::WARN; // Default to WARN
    static bool initialized = false;
    if (!initialized) {
        const char* env_level = std::getenv("CAFMAKER_LOG_LEVEL");
        if (env_level) {
            std::string s_level(env_level);
            if (s_level == "DEBUG") level = LogLevel::DEBUG;
            else if (s_level == "INFO") level = LogLevel::INFO;
            else if (s_level == "WARN") level = LogLevel::WARN;
            else if (s_level == "CRITICAL") level = LogLevel::CRITICAL;
        }
        initialized = true;
    }
    return level;
}

#define LOG_DEBUG(msg) do { if (get_global_log_level() <= LogLevel::DEBUG) { std::cout << "[DEBUG] " << msg; } } while(0)
#define LOG_INFO(msg) do { if (get_global_log_level() <= LogLevel::INFO) { std::cout << "[INFO] " << msg; } } while(0)
#define LOG_WARN(msg) do { if (get_global_log_level() <= LogLevel::WARN) { std::cerr << "\033[33m[WARN]\033[0m " << msg; } } while(0)
#define LOG_CRITICAL(msg) do { if (get_global_log_level() <= LogLevel::CRITICAL) { std::cerr << "\033[31m[CRITICAL]\033[0m " << msg; } } while(0)

#endif