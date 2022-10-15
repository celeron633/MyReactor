#ifndef __EVENT_LOOP_THREAD__
#define __EVENT_LOOP_THREAD__

#include "Mutex.hh"
#include "CurrentThread.hh"
#include "EventLoop.hh"

#include <pthread.h>

using net::EventLoop;

class EventLoopThread : base::NonCopyable {
public:
    EventLoopThread();
    ~EventLoopThread();

    void start();
    void stop();
    void join();

    static void* loopThreadRoutine(void* args);
    EventLoop* getLoop();
private:
    EventLoop* _loop;
    pthread_t _thread;
    bool _started;
};

#endif