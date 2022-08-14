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
    FATAL
};

namespace base {

class Logger {
public:
    static Logger * GetInstance() {
        static Logger logger;
        return &logger;
    }
public:
    void Append(string str);
    void Append(string str, const char *func, int lineNo);
    void SetLogLevel(int logLevel);
    int GetLogLevel();
    void SetFilterLevel(int level);
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

#define LOG_DEBUG(fmt, ...)                         \
do {                                                \
    char buf[2048] = {0};                           \
    sprintf(buf, fmt, ##__VA_ARGS__);               \
    base::Logger *logger = Logger::GetInstance();   \
    logger->SetLogLevel(TraceLevels::DEBUG);        \
    logger->Append(buf, __FUNCTION__, __LINE__);    \
} while (0);                                        \

#define LOG_INFO(fmt, ...)                          \
do {                                                \
    char buf[2048] = {0};                           \
    sprintf(buf, fmt, ##__VA_ARGS__);               \
    base::Logger *logger = Logger::GetInstance();   \
    logger->SetLogLevel(TraceLevels::INFO);         \
    logger->Append(buf, __FUNCTION__, __LINE__);    \
} while (0);                                        \

#define LOG_TRACE(fmt, ...)                         \
do {                                                \
    char buf[2048] = {0};                           \
    sprintf(buf, fmt, ##__VA_ARGS__);               \
    base::Logger *logger = Logger::GetInstance();   \
    logger->SetLogLevel(TraceLevels::TRACE);        \
    logger->Append(buf, __FUNCTION__, __LINE__);    \
} while (0);                                        \

#define LOG_WARN(fmt, ...)                          \
do {                                                \
    char buf[2048] = {0};                           \
    sprintf(buf, fmt, ##__VA_ARGS__);               \
    base::Logger *logger = Logger::GetInstance();   \
    logger->SetLogLevel(TraceLevels::WARN);         \
    logger->Append(buf, __FUNCTION__, __LINE__);    \
} while (0);                                        \

#define LOG_ERROR(fmt, ...)                         \
do {                                                \
    char buf[2048] = {0};                           \
    sprintf(buf, fmt, ##__VA_ARGS__);               \
    base::Logger *logger = Logger::GetInstance();   \
    logger->SetLogLevel(TraceLevels::ERROR);        \
    logger->Append(buf, __FUNCTION__, __LINE__);    \
} while (0);                                        \

#define LOG_FATAL(fmt, ...)                         \
do {                                                \
    char buf[2048] = {0};                           \
    sprintf(buf, fmt, ##__VA_ARGS__);               \
    base::Logger *logger = Logger::GetInstance();   \
    logger->SetLogLevel(TraceLevels::FATAL);        \
    logger->Append(buf, __FUNCTION__, __LINE__);    \
} while (0);                                        \

#endif