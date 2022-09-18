#include "EPollPoller.hh"
#include "Log.hh"

#include <vector>

using namespace net;
using namespace base;
using namespace std;

const int kEpollSize = 1024;
const int kEpollPollTmoMs = 3 * 1000;

// TODO: why use loop to initialize base class
EPollPoller::EPollPoller(EventLoop *eventLoop) : Poller(eventLoop), _eventsNum(0)
{
    _epollFd = epoll_create(kEpollSize);
    LOG_DEBUG("_epollFd: [%d]", _epollFd);

    _epEvents.reserve(kEpollSize);
}

EPollPoller::~EPollPoller()
{
    
}

void EPollPoller::FillActiveChannels(ChannelList *channelList)
{
    LOG_DEBUG("FillActiveChannels begin!");

    for (int i = 0; i < _eventsNum; ++i) {
        // get 'Channel *' pointer from epoll_data
        Channel *channel = (Channel *)_epEvents[i].data.ptr;
        LOG_DEBUG("channel addr: %p", channel);
        channel->SetrEvents(_epEvents[i].events);

        channelList->push_back(channel);
    }
    LOG_DEBUG("FillActiveChannels end");
}

Timestamp EPollPoller::Poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_DEBUG("Poll begin");

    _eventsNum = epoll_wait(_epollFd, _epEvents.data(), kEpollSize, timeoutMs);
    Timestamp retStamp;
    LOG_DEBUG("eventsNum: [%d]", _eventsNum);

    if (_eventsNum == 0) {
        LOG_DEBUG("Poll timeout");
    } else if (_eventsNum > 0) {
        FillActiveChannels(activeChannels);
    } else {
        if (errno != EINTR) {
            LOG_ERROR("epoll_wait error");
            perror("epoll_wait");
        } else {
            LOG_WARN("EINTR");
        }
    }

    LOG_DEBUG("Poll end");
    return retStamp;
}

void EPollPoller::UpdateChannel(Channel *channel)
{
    int events = channel->GetEvents();
    LOG_DEBUG("events: [%s]", Channel::Events2String(events).c_str());

    int fd = channel->GetFd();
    LOG_DEBUG("fd: [%d]", channel->GetFd());

    struct epoll_event epEvent;
    epEvent.data.ptr = (void *)channel;
    epEvent.events = events;

    // add (regist this channel to epoll object)
    if (!HasChannel(channel)) {
        LOG_INFO("channel [%d] does not exist! call EPOLL_CTL_ADD", fd);
        if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &epEvent) < 0) {
            LOG_ERROR("epoll_ctl EPOLL_CTL_ADD failed");
            perror("epoll_ctl");
        } else {
            _channelMap.insert(pair<int, Channel*>(fd, channel));
        }
    }

    // update existed 
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &epEvent) < 0) {
        LOG_ERROR("epoll_ctl EPOLL_CTL_MOD failed");
        perror("epoll_ctl");
    }
}

void EPollPoller::RemoveChannel(Channel *channel)
{
    int fd = channel->GetFd();
    LOG_DEBUG("fd: [%d]", channel->GetFd());

    int events = channel->GetEvents();
    LOG_DEBUG("events: [%s]", Channel::Events2String(events).c_str());

    struct epoll_event epEvent;
    epEvent.data.ptr = (void *)channel;
    epEvent.events = events;

    if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &epEvent) < 0) {
        LOG_ERROR("epoll_ctl EPOLL_CTL_DEL failed!");
    } else {
        auto it = _channelMap.find(fd);
        if (it != _channelMap.end()) {
            _channelMap.erase(it);
        }
    }
    return;
}