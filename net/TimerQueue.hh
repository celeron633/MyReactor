#ifndef __TIMER_QUEUE_HH__
#define __TIMER_QUEUE_HH__

#include "Timestamp.hh"
#include "Timer.hh"

#include <list>

using std::list;
using std::pair;

namespace net {

class EventLoop;

typedef pair<Timestamp, Timer*> TimerEntry;
typedef list<TimerEntry> TimerList;
using TimerSeqType = int64_t;
class Timer;

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

    void makeInvalid()
    {
        this->_sequence = -1;
        this->_timerPtr = NULL;
    }
private:
    Timer* _timerPtr;
    int64_t _sequence;
};

class TimerQueue {
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
public:
    void deleteTimer(TimerId timerId);
    void deleteAllTimers(void);
    void deleteAllCanceledTimers(void);
    TimerId addTimer(const TimerCallback& cb, Timestamp when, unsigned int interval, int repeatCount = -1);
    void runTimers();
private:
    void deleteTimerInLoop(TimerId timerId);
    void deleteAllTimersInLoop(void);
    void deleteAllCanceledTimersInLoop(void);
    void addTimerInLoop(Timer* t);
private:
    EventLoop* _eventLoop;
    TimerList _timerList;
};

}

#endif