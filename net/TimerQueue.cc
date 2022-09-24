#include "TimerQueue.hh"
#include "EventLoop.hh"
#include "Log.hh"

#include <assert.h>

using namespace base;
using namespace net;
using namespace std;

TimerQueue::TimerQueue(EventLoop* loop) : _eventLoop(loop)
{
    assert(loop != NULL);
}

TimerQueue::~TimerQueue()
{
    
}

void TimerQueue::deleteTimer(TimerId timerId)
{
    this->_eventLoop->RunInLoopThread(bind(&TimerQueue::deleteTimerInLoop, this, timerId));
}

void TimerQueue::deleteAllTimers(void)
{
    this->_eventLoop->RunInLoopThread(bind(&TimerQueue::deleteAllTimersInLoop, this));
}

void TimerQueue::deleteAllCanceledTimers(void)
{
    this->_eventLoop->RunInLoopThread(bind(&TimerQueue::deleteAllCanceledTimersInLoop, this));
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, unsigned int interval, int repeatCount)
{
    Timer *timer = new Timer(cb, when, interval, repeatCount);
    this->_eventLoop->RunInLoopThread(bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->getSequence());
}

void TimerQueue::runTimers()
{
    this->_eventLoop->AssertInEventLoopThread();

    if (this->_timerList.empty()) {
        return;
    }

    Timestamp now;
    for (auto it = _timerList.begin(); it != _timerList.end(); ++it) {
        if (it->second->canceled()) {
            continue;
        }

        if (now < it->second->getExpiration()) {
            continue;
        }

        it->second->run();  // invoke timer
    }
}

void TimerQueue::deleteTimerInLoop(TimerId timerId)
{
    // run within loop thread
    this->_eventLoop->AssertInEventLoopThread();

    for (auto it = _timerList.begin(); it != _timerList.end(); ++it) {
        if (it->second == timerId.getTimer()) {
            LOG_DEBUG("removing timer [%ld]...", it->second->getSequence());
            delete it->second;  // delete 'new' allocated space
            _timerList.erase(it);
            break;
        }
    }
}

void TimerQueue::deleteAllTimersInLoop(void)
{
    // run within loop thread
    this->_eventLoop->AssertInEventLoopThread();

    for (auto it = _timerList.begin(); it != _timerList.end(); ) {
        LOG_DEBUG("removing timer [%ld]...", it->second->getSequence());
        delete it->second;
        it = this->_timerList.erase(it);
    }
}

 void TimerQueue::deleteAllCanceledTimersInLoop(void)
 {
    // run within loop thread
    this->_eventLoop->AssertInEventLoopThread();

    for (auto it = _timerList.begin(); it != _timerList.end(); ) {
        if (it->second->canceled()) {
            LOG_DEBUG("removing timer [%ld]...", it->second->getSequence());
            delete it->second;
            it = this->_timerList.erase(it);
        } else {
            ++it;
        }
    }
 }

void TimerQueue::addTimerInLoop(Timer* t)
{
    this->_eventLoop->AssertInEventLoopThread();

    this->_timerList.push_back(make_pair(t->getExpiration(), t)); // add to list
}
