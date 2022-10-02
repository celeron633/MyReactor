#ifndef __POLL_POLLER_HH__
#define __POLL_POLLER_HH__

#include "Poller.hh"
#include <sys/poll.h>

#define POLL_SIZE 1024

namespace net {

using FdsVec = std::vector<struct pollfd>;
using FdChannelPtrMap = std::map<int, Channel*>;

class PollPoller : public Poller {
public:
    PollPoller(EventLoop *loop);
    ~PollPoller();

    // implement interfaces
    virtual Timestamp Poll(int timeoutMs, ChannelList *activeChannels);
    virtual bool UpdateChannel(Channel *channel);
    virtual bool RemoveChannel(Channel *channel);

    static void ShowPollFd(const struct pollfd* pfd);
private:
    void FillActiveChannels(ChannelList *channelList);
private:
    int _eventsNum;
    FdsVec _fds;
    FdChannelPtrMap _fdChannelPtrMap;
};

};

#endif