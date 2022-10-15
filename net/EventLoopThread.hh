#ifndef __EVENT_LOOP_THREAD__
#define __EVENT_LOOP_THREAD__

#include "Mutex.hh"
#include "CurrentThread.hh"
#include "EventLoop.hh"

using net::EventLoop;

class EventLoopThread {
public:
    EventLoopThread();
    ~EventLoopThread();

    void start();
    void stop();
private:
    EventLoop* _loop;
};

#endif