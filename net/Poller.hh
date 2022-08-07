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

// fd -> channel
typedef map<int, Channel*> ChannelMap;

class Poller : NonCopyable {
public:
    Poller(EventLoop *eventLoop)
    {
        _eventLoop = eventLoop;
    }
    virtual ~Poller();

    // derived poller class need to implement such interfaces
    virtual Timestamp Poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void UpdateChannel(Channel *channel) = 0;
    virtual void RemoveChannel(Channel *channel) = 0;

    // whether a channel has already registed to the poller
    bool HasChannel(Channel *channel);
protected:
    // fd mapping to channel
    ChannelMap _channelMap;

private:
    EventLoop *_eventLoop;
};

}

#endif