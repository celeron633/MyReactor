#ifndef __EVENT_LOOP_THREAD__
#define __EVENT_LOOP_THREAD__

#include "Mutex.hh"
#include "CurrentThread.hh"
#include "EventLoop.hh"

#include <pthread.h>

using net::EventLoop;

class EventLoopThread {
public:
    EventLoopThread();
    ~EventLoopThread();

    void start();
    void stop();
    void join();

    static void* loopThreadRoutine(void* args);
private:
    EventLoop* _loop;
    pthread_t _thread;
};

#endif