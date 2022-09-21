#ifndef __TIMER_ID_HH__
#define __TIMER_ID_HH__

#include "Timer.hh"
#include "TimerQueue.hh"

namespace net {

// wrapper for 'Timer'
class TimerId {
    friend class TimerQueue;
public:
    TimerId() : _timerPtr(NULL), _sequence(0)
    {

    }

    TimerId(Timer* t, int64_t seq) : _timerPtr(t), _sequence(seq)
    {

    }

    Timer* getTimer()
    {
        return _timerPtr;
    }

    int64_t getSeq()
    {
        return _sequence;
    }
private:
    Timer* _timerPtr;
    int64_t _sequence;
};

}

#endif