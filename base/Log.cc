#include "Log.hh"

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
    *addr = 0;
}

void Logger::Append(string str)
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

    printf(" %s@%d", func, lineNo);
    // when printf end with a newline, the previous buffered content will output
    printf("-> %s\n", str.c_str());
}

/*
int main(int argc, char **argv)
{
    LOG_DEBUG("%s", "123");
    LOG_INFO("123 %s", "hello world");
    LOG_FATAL("123 %s", "hello world");
    return 0;
} */