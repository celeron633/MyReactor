#ifndef __TIMESTAMP_HH__
#define __TIMESTAMP_HH__

#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

#include <cstdio>
#include <iostream>
#include <string>

using std::string;

namespace base
{

class Timestamp {
public:
    Timestamp();
    ~Timestamp();

    void SetToNow();
    string ConvertToString();
    static string GetCurrentTimestamp();

    uint64_t GetMilliseconds() const;
    void AddMilliseconds(uint64_t i);

    // operators
    Timestamp& operator+=(int milliseconds);
    bool operator<(const Timestamp& r);
    bool operator==(const Timestamp& r);
private:
    // timeval holder
    timeval _timeval;
};

}

#endif