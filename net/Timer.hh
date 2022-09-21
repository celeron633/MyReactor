#ifndef __TIMER_HH__
#define __TIMER_HH__

#include "Timestamp.hh"
#include "Callbacks.hh"
#include "Log.hh"

#include <stdint.h>
#include <atomic>

using base::Timestamp;

namespace net {

class Timer {

public:
    Timer(TimerCallback cb, Timestamp when, int interval, int repeatCount = -1);
    ~Timer();

    void run();

    void cancel()
    {
        this->_canceled = true;
    }
    bool canceled()
    {
        return _canceled;
    }

    Timestamp getExpiration(void)
    {
        return _expiration;
    }

    int getRepeatCount()
    {
        return _repeatCount;
    }

private:
    TimerCallback _callback;
    Timestamp _expiration;
    int _interval;
    int _repeatCount;
    bool _canceled; // if canceled

    int64_t _sequence;

    static std::atomic_int64_t _timerSeq;
};

}

#endif