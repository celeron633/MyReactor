#include <assert.h>

#include "EventLoop.hh"
#include "Poller.hh"
#include "EPollPoller.hh"
#include "Channel.hh"
#include "Log.hh"

using namespace std;
using namespace base;
using namespace net;

#define USE_BUILTIN_ASSERT

const int kPollTimeoutMs = 3 * 1000;

EventLoop::EventLoop() : _looping(false), _quit(false), \
    _tid(CurrentThread::GetThreadId())
{
    LOG_INFO("EventLoop object constructed, tid:[%d]", this->_tid);
    _poller = new EPollPoller(this);
}

EventLoop::~EventLoop()
{
    LOG_INFO("EventLoop object destructed!");
}

void EventLoop::loop()
{
    LOG_INFO("loop begin");
    if (_looping) {
        return;
    }

    // set flags
    _looping = true;
    _quit = false;

    while (_looping) {
        // 1. handle poller event
        _activeChannels.clear();
        Timestamp pollRetTime = _poller->Poll(kPollTimeoutMs, &_activeChannels);
        LOG_INFO("pollRetTime: [%s]", pollRetTime.ConvertToString().c_str());

        LOG_DEBUG("activeChannels' count: [%lu]", _activeChannels.size());
        for (ChannelList::iterator it = _activeChannels.begin(); it != _activeChannels.end(); ++it) {
            (*it)->HandleEvent(pollRetTime);
        }

        // 2. handle pending functors
        _mutex.Lock();
        if (!_pendingFunctors.empty()) {
            for (auto it = _pendingFunctors.begin(); it != _pendingFunctors.end(); ) {
                (*(it))();  // call functor
                it = _pendingFunctors.erase(it);    // then remove it from task queue
            }
        }
        _mutex.Unlock();

        // 3. handle cron(timer) job
        // TODO: servCron
    }
    
    LOG_INFO("loop end");
}

void EventLoop::stop()
{
    _looping = false;
    _quit = true;
}

// run callback in eventloop
void EventLoop::RunInLoopThread(Functor func)
{
    pid_t currentTid = CurrentThread::GetThreadId();
    
    // caller just in this loop thread
    if (currentTid == _tid) {
        func();
    } else {
        QueueInLoop(func);
    }
}

void EventLoop::QueueInLoop(Functor func)
{
    // TODO: make loop return from 'Poll' immediately, we need a eventFd to get this done

    _mutex.Lock();
    _pendingFunctors.emplace_back(func);
    _mutex.Unlock();
}

void EventLoop::UpdateChannel(Channel* channel)
{
    this->_poller->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* ch)
{
    _poller->RemoveChannel(ch);
}

void EventLoop::AssertInEventLoopThread(void)
{
#ifdef USE_BUILTIN_ASSERT
    assert(CurrentThread::GetThreadId() == this->_tid);
#else
    if (CurrentThread::GetThreadId() != this->_tid) {
        LOG_FATAL("AssertInEventLoopThread!");
    }
#endif
}

bool EventLoop::InEventLoopThread(void)
{
    return CurrentThread::GetThreadId() == this->_tid;
}