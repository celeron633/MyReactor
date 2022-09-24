#ifndef __TIMER_HH__
#define __TIMER_HH__

#include "Timestamp.hh"
#include "Callbacks.hh"
#include "Log.hh"
#include "NonCopyable.hh"

#include <stdint.h>
#include <atomic>

using base::Timestamp;
using base::NonCopyable;

namespace net {

class Timer : public NonCopyable {
    friend class TimerQueue;
public:
    Timer(TimerCallback cb, Timestamp when, unsigned int interval, int repeatCount = -1);
    ~Timer();

    void run(); // run callback
    void cancel();  // cancel this timer
    bool canceled();    // if canceled

    Timestamp getExpiration(void);
    int getRepeatCount();
    int64_t getSequence();

private:
    TimerCallback _callback;
    Timestamp _expiration;
    unsigned int _interval;
    int _repeatCount;
    bool _canceled; // if canceled

    int64_t _sequence;

    static std::atomic_int64_t _timerSeq;
};

}

#endif