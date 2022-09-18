#ifndef __EPOLL_POLLER_HH__
#define __EPOLL_POLLER_HH__

#include "Poller.hh"

#include <vector>

using std::vector;

namespace net {

typedef vector<epoll_event> EpollEvents;

class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller();
public:
    // 实现父类的接口
    virtual Timestamp Poll(int timeoutMs, ChannelList *activeChannels);
    virtual void UpdateChannel(Channel *channel);
    virtual void RemoveChannel(Channel *channel);
private:
    void FillActiveChannels(ChannelList *channelList);
private:
    int _epollFd;
    EpollEvents _epEvents;
    int _eventsNum;
};

};


#endif