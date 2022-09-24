#ifndef __LOG_HH__
#define __LOG_HH__

#include "Timestamp.hh"

#include <iostream>
#include <cstdio>
#include <string>

using std::string;

enum TraceLevels {
    DEBUG,
    INFO,
    TRACE,
    WARN,
    ERROR,
    SYSE,
    FATAL,
    CRITICAL
};

namespace base {

class Logger {
public:
    static Logger* GetInstance() {
        static Logger logger;
        return &logger;
    }
public:
    // void Append(string str);
    // void Append(string str, const char *func, int lineNo);
    void Append(int level, const char* file, const char* func, int line, const char* fmt, ...);
    void MakeHeader(string& str, int level, const char* file, const char* func, int line);
    void SetLogLevel(int logLevel);
    int GetLogLevel();
    void SetFilterLevel(int level);
    void Crash();   // raise a SIGSEGV crash
private:
    Logger();
    ~Logger();
    Logger(const Logger &r) = delete;   // non-copyable
    Logger & operator=(const Logger &r) = delete;   // non-copyable

private:
    int _logLevel;
    int _filterLevel;
};

}

#define LOG_DEBUG(fmt, ...)                                                                                 \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(DEBUG, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);       \
} while (0)

#define LOG_INFO(fmt, ...)                                                                                  \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(INFO, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);        \
} while (0)

#define LOG_TRACE(fmt, ...)                                                                                 \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(TRACE, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);       \
} while (0)

#define LOG_WARN(fmt, ...)                                                                                  \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(WARN, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);        \
} while (0)

#define LOG_ERROR(fmt, ...)                                                                                 \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(ERROR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);       \
} while (0)

#define LOG_SYSE(fmt, ...)                                                                                  \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(SYSE, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);        \
} while (0)

#define LOG_FATAL(fmt, ...)                                                                                 \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(FATAL, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);       \
} while (0)

#define LOG_CRITICAL(fmt, ...)                                                                              \
do {                                                                                                        \
    base::Logger::GetInstance()->Append(CRITICAL, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);    \
} while (0)

#endif