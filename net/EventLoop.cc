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

EventLoop::EventLoop() : _looping(false), _quit(false), _tid(CurrentThread::GetThreadId())
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

        for (ChannelList::iterator it = _activeChannels.begin(); it != _activeChannels.end(); ++it) {
            (*it)->HandleEvent(pollRetTime);
        }

        // 2. handle pending functors
        _mutex.Lock();
        if (!_pendingFunctors.empty()) {
            for (auto &it : _pendingFunctors) {
                it();
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
void EventLoop::RunInLoop(Functor func)
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

void EventLoop::AssertInEventLoop(void)
{
#ifdef USE_BUILTIN_ASSERT
    assert(CurrentThread::GetThreadId() == this->_tid);
#else
    if (CurrentThread::GetThreadId() != this->_tid) {
        LOG_FATAL("AssertInEventLoop!");
    }
#endif
}


int main(int argc, char *argv[])
{
    EventLoop loop;
    printf("tid: [%ld]\n", syscall(SYS_gettid));
    loop.AssertInEventLoop();
    loop.loop();
    return 0;
}
