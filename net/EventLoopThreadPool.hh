#ifndef __EVENT_LOOP_THREAD_POLL_HH__
#define __EVENT_LOOP_THREAD_POLL_HH__

#include "EventLoopThread.hh"

#include <vector>

const int kLoopThreadPoolSize = 4;

namespace net {

class EventLoopThreadPool {
public:
    EventLoopThreadPool();
    ~EventLoopThreadPool();

    void start();
    // void stop(); // this member method is unnecessary
    EventLoopThread* getNextLoop();
private:
    vector<EventLoopThread*> _loops;
    int _curIndex;
};

};

#endif