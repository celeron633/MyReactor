#ifndef __EVENTLOOP_HH__
#define __EVENTLOOP_HH__

#include "NonCopyable.hh"
#include "Mutex.hh"
#include "CurrentThread.hh"

#include <functional>
#include <vector>
#include <atomic>

#include <unistd.h>

using std::vector;
using std::function;

using base::NonCopyable;

typedef function<void ()> Functor;

namespace net {

class Poller;
class Channel;

class EventLoop : NonCopyable {
public:
    EventLoop();
    ~EventLoop();

    void UpdateChannel(Channel *channel);
    // 开始事件循环
    void loop();
    // 结束事件循环
    void stop();
    // 在EventLoop中运行
    void RunInLoop();
    // 加入待执行回调队列
    void QueueInLoop();

private:
    Poller *_poller;
    vector<Functor> _pendingFunctors;

    // 状态相关
    std::atomic_bool _looping;
    std::atomic_bool _stopped;

    // 互斥锁
    base::Mutex _mutex;

    // 本loop的tid, only Linux
    const pid_t _tid;
};

};

#endif