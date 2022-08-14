#ifndef __EVENT_LOOP_HH__
#define __EVENT_LOOP_HH__

#include "NonCopyable.hh"
#include "Mutex.hh"
#include "CurrentThread.hh"

#include <functional>
#include <vector>
#include <atomic>

#include <unistd.h>
#include <sys/syscall.h>

using std::vector;
using std::function;
using base::NonCopyable;

namespace net {

class Poller;
class Channel;

typedef function<void (void)> Functor;
// channel list
typedef vector<Channel *> ChannelList;

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
    void RunInLoop(Functor func);
    // 加入待执行回调队列
    void QueueInLoop(Functor func);

private:
    Poller *_poller;
    vector<Functor> _pendingFunctors;

    // 状态相关
    std::atomic_bool _looping;
    std::atomic_bool _quit;

    // 互斥锁
    base::Mutex _mutex;

    // 本loop的tid, only Linux
    const pid_t _tid;

    // active channels
    ChannelList _activeChannels;
};

};

#endif