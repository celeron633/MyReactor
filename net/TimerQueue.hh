#ifndef __TIMER_QUEUE_HH__
#define __TIMER_QUEUE_HH__

#include "Timestamp.hh"
#include "Timer.hh"
#include "TimerId.hh"

#include <list>

using std::list;
using std::pair;

namespace net {

class EventLoop;

typedef pair<Timestamp, Timer*> TimerEntry;
typedef list<TimerEntry> TimerList;

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