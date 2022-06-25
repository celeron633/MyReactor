#ifndef __POLLER_HH__
#define __POLLER_HH__

#include "Channel.hh"
#include "EventLoop.hh"

#include <vector>
#include <map>

using std::vector;
using std::map;

namespace net {

class EventLoop;

typedef vector<Channel *> ChannelList;
typedef map<int, Channel*> ChannelMap;

class Poller {
public:
    Poller(EventLoop *eventLoop);
    virtual ~Poller();

    // 子类需要实现这些纯虚函数
    virtual Timestamp Poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void UpdateChannel(Channel *channel) = 0;
    virtual void RemoveChannel(Channel *channel) = 0;

    // Poller是否注册了对应的channel
    bool HasChannel(Channel *channel);
private:
    ChannelMap _channelMap;
};

}

#endif