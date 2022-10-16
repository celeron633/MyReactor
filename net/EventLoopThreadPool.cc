#include "EventLoopThreadPool.hh"
#include "Log.hh"

#include <limits.h>

using namespace net;
using namespace base;
using namespace std;

EventLoopThreadPool::EventLoopThreadPool() : _curIndex(0)
{
    for (auto i = 0; i < kLoopThreadPoolSize; ++i) {
        EventLoopThread* th = new EventLoopThread();
        this->_loops.push_back(th); // add pointer of object to vector (object is NonCopyable)
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // this->stop();
    for (auto i = 0; i < kLoopThreadPoolSize; ++i) {
        delete _loops[i];   // destructor will stop loopThread first
        _loops[i] = NULL;   // mark as NULL
    }
}

void EventLoopThreadPool::start()
{
    for (auto i = 0; i < kLoopThreadPoolSize; ++i) {
        _loops[i]->start(); // block call until thread is ready
    }

    LOG_INFO("EventLoopThreadPool::start() OK");
}

/*
void EventLoopThreadPool::stop()
{
    for (auto i = 0; i < kLoopThreadPoolSize; ++i) {
        _loops[i]->stop();
    } 
}
*/

EventLoopThread* EventLoopThreadPool::getNextLoop()
{
    auto idx = _curIndex % kLoopThreadPoolSize;
    return _loops[idx]; // get different loop each time
    ++_curIndex;

    // prevent int overflow
    if (_curIndex == INT32_MAX) {
        _curIndex = 0;
    }
}