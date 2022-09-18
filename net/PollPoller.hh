#ifndef __POLL_POLLER_HH__
#define __POLL_POLLER_HH__

#include "Poller.hh"
#include <sys/poll.h>

#define POLL_SIZE 1024

namespace net {

class PollPoller : public Poller {
public:
    PollPoller(EventLoop *loop);
    ~PollPoller();

    // implement interfaces
    virtual Timestamp Poll(int timeoutMs, ChannelList *activeChannels);
    virtual bool UpdateChannel(Channel *channel);
    virtual bool RemoveChannel(Channel *channel);
private:
    void FillActiveChannels(ChannelList *channelList);
private:
    int _eventsNum;
    pollfd fds[POLL_SIZE];
};

};

#endif