#pragma once
#include <cstdarg>
#include <cstdio>

enum class LogLevel {
    DEBUG,
    INFO,
};

class Logger {
public:
    static void debug(const char* fmt, ...) {
        if (level > LogLevel::DEBUG) {
            return;
        }
        va_list args;
        va_start(args, fmt);
        vfprintf(stdout, fmt, args);
        va_end(args);
    }

    static void info(const char* fmt, ...) {
        if (level > LogLevel::INFO) {
            return;
        }
        va_list args;
        va_start(args, fmt);
        vfprintf(stdout, fmt, args);
        va_end(args);
    }

public:
    static LogLevel level;
};
