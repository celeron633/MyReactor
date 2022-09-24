#include "Log.hh"

#include <stdarg.h>
#include <libgen.h>

using namespace base;

Logger::Logger()
{
    // by default, output everything
    _filterLevel = TraceLevels::DEBUG;
}

Logger::~Logger()
{

}

void Logger::SetLogLevel(int level)
{
    this->_logLevel = level;
}

int Logger::GetLogLevel()
{
    return _logLevel;
}

void Logger::SetFilterLevel(int level)
{
    _filterLevel = level;
}

void Logger::Crash()
{
    int* addr = 0x0;
    *addr = 0;  // this will definitely cause a crash
}

/* void Logger::Append(string str)
{
    // level filter
    if (_filterLevel > _logLevel) {
        return;
    }

    // timestamp
    printf("%s ", Timestamp::GetCurrentTimestamp().c_str());

    switch (this->_logLevel) {
    case TraceLevels::DEBUG :
        printf("[DEBUG]");
        break;
    case TraceLevels::INFO :
        printf("[INFO]");
        break;
    case TraceLevels::TRACE :
        printf("[TRACE]");
        break;
    case TraceLevels::WARN :
        printf("[WARN]");
        break;
    case TraceLevels::ERROR :
        printf("[ERROR]");
        break;
    case TraceLevels::FATAL :
        printf("[FATAL]");
        break;
    default:
        break;
    }
    // when printf end with a newline, the previous buffered content will output
    printf(" : %s\n", str.c_str());
}

void Logger::Append(string str, const char *func, int lineNo)
{
    // level filter
    if (_filterLevel > _logLevel) {
        return;
    }

    // timestamp
    printf("%s ", Timestamp::GetCurrentTimestamp().c_str());

    switch (this->_logLevel) {
    case TraceLevels::DEBUG :
        printf("[DEBUG]");
        break;
    case TraceLevels::INFO :
        printf("[INFO]");
        break;
    case TraceLevels::TRACE :
        printf("[TRACE]");
        break;
    case TraceLevels::WARN :
        printf("[WARN]");
        break;
    case TraceLevels::ERROR :
        printf("[ERROR]");
        break;
    case TraceLevels::FATAL :
        printf("[FATAL]");
        break;
    default:
        break;
    }

    printf(" %s@LINE[%d]", func, lineNo);
    // when printf end with a newline, the previous buffered content will output
    printf("-> %s\n", str.c_str());
} */

void Logger::MakeHeader(string& str, int level, const char* file, const char* func, int line)
{
    Timestamp now;
    str.append(now.ConvertToString());
    str.append(" [");
    switch (level) {
        case DEBUG:
            str.append("DEBUG");
            break;
        case INFO:
            str.append("INFO");
            break;
        case TRACE:
            str.append("TRACE");
            break;
        case WARN:
            str.append("WARN");
            break;
        case ERROR:
            str.append("ERROR");
            break;
        case SYSE:
            str.append("SYSE");
            break;
        case FATAL:
            str.append("FATAL");
            break;
        case CRITICAL:
            str.append("CRITICAL");
            break;
        default:
            str.append("UNKNOWN");
            break;
    }
    str.append("] ");
    str.append(func).append("@").append(basename((char*)file)).append(":").append(std::to_string(line)).append("-> ");

    return;
}


void Logger::Append(int level, const char* file, const char* func, int line, const char* fmt, ...)
{
    if (level < _filterLevel) {
        return;
    }
    unsigned int logLineLen = 0;
    string logHeader;
    string logLine;
    string logLineForOut;

    // 1. add a header
    MakeHeader(logHeader, level, file, func, line);

    // 2. get the length of formated logLine
    va_list ap;
    va_start(ap, fmt);
    logLineLen = vsnprintf(NULL, 0, fmt, ap);
    // printf("%d\n", logLineLen);
    va_end(ap);

    // 3. reserve space
    if (logLine.capacity() <= logLineLen + 1) {
        logLine.resize(logLineLen + 1);
    }

    // 4. append logLine to string
    va_list aq;
    va_start(aq, fmt);
    vsnprintf((char *)logLine.data(), logLine.capacity(), fmt, aq); // won't affect length()
    va_end(aq);

    // logLine.append("\n"); won't work
    logLineForOut = logLine.c_str();
    logLineForOut.append("\n");

    // output to stdout
    std::cout << logHeader << logLineForOut;

    if (level == FATAL || level == CRITICAL) {
        Crash();
    }

    return;
}
