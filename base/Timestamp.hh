#ifndef __TIME_STAMP_HH_
#define __TIME_STAMP_HH_

#include <unistd.h>
#include <sys/time.h>

#include <cstdio>
#include <iostream>
#include <string>

using std::string;

namespace base
{

class Timestamp {
public:
    Timestamp()
    {
        gettimeofday(&_timeval, NULL);
    }

    void SetToNow()
    {
        gettimeofday(&_timeval, NULL);
    }

    string ConvertToString()
    {
        string timeStr;
        char buf[128] = {0};

        gettimeofday(&_timeval, NULL);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S.", localtime(&(_timeval.tv_sec)));
        timeStr.append(buf);
        // add ms
        sprintf(buf, "%ld", _timeval.tv_usec / 1000);
        timeStr.append(buf);

        return timeStr;
    }

    static string GetCurrentTimestamp()
    {
        string timeStr;
        char buf[128] = {0};
        timeval tv;

        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S.", localtime(&(tv.tv_sec)));
        timeStr.append(buf);
        sprintf(buf, "%ld", tv.tv_usec / 1000);
        timeStr.append(buf);

        return timeStr;
    }
private:
    // timeval holder
    timeval _timeval;
};

}

#endif