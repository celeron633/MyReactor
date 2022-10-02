#include "PollPoller.hh"

using namespace net;
using namespace base;
using namespace std;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop), _eventsNum(0)
{
    // this->_fds.resize(POLL_SIZE); // no need
}

PollPoller::~PollPoller()
{

}

Timestamp PollPoller::Poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_DEBUG("Poll begin");
    int pollRet = 0;

    pollRet = poll(this->_fds.data(), this->_eventsNum, timeoutMs);
    LOG_DEBUG("pollRet = %d", pollRet);
    if (pollRet < 0) {
        LOG_SYSE("poll return negative!");
        perror("poll");
    }

    if (pollRet > 0) {
        this->_eventsNum = pollRet;
        FillActiveChannels(activeChannels);
    }

    LOG_DEBUG("Poll end");

    Timestamp afterPoll;
    return afterPoll;
}

bool PollPoller::UpdateChannel(Channel *channel)
{
    LOG_DEBUG("UpdateChannel begin");
    LOG_DEBUG("index: [%d], fd: [%d], events: [%d]", channel->GetIndex(), channel->GetFd(), channel->GetEvents());
    if (this->_fdChannelPtrMap.find(channel->GetFd()) != this->_fdChannelPtrMap.end() && channel->GetIndex() != -1) {
        LOG_SYSE("channel in _fdChannelPtrMap while index is -1, fd: [%d]", channel->GetFd());
        return false;
    }

    // add 
    if (channel->GetIndex() == -1) {
        channel->SetIndex(this->_fds.size());

        struct pollfd pfd;
        pfd.fd = channel->GetFd();
        pfd.events = channel->GetEvents();
        this->_fds.push_back(pfd);

        channel->SetIndex(_fds.size());
        _fdChannelPtrMap.insert(make_pair(channel->GetFd(), channel));
    } else {    // update
        for (auto it = _fds.begin(); it != _fds.end(); ++it) {
            if (it->fd == channel->GetFd()) {
                it->events = channel->GetEvents();
                break;
            }
        }
    }

    LOG_DEBUG("UpdateChannel end");

    return true;
}

bool PollPoller::RemoveChannel(Channel *channel)
{
    LOG_DEBUG("RemoveChannel begin!");
    bool pfdRemoved = false; 

    for (auto it = _fds.begin(); it != _fds.end(); ) {
        if (it->fd == channel->GetFd()) {
            it = _fds.erase(it);
            pfdRemoved = true;
        } else {
            ++it;
        }
    }

    if (pfdRemoved) {
        auto it = _fdChannelPtrMap.find(channel->GetFd());
        _fdChannelPtrMap.erase(it);
        channel->SetIndex(-1);
        LOG_DEBUG("fd: [%d] removed from _fds", channel->GetFd());
    } else {
        LOG_SYSE("visited all pollfd in _fds, but can not find fd: [%d]", channel->GetFd());
        return false;
    }

    LOG_DEBUG("RemoveChannel end");
    return true;
}

void PollPoller::FillActiveChannels(ChannelList *channelList)
{
    for (auto it = _fds.begin(); it != _fds.end() && this->_eventsNum > 0; ++it) {
        if (it->revents > 0) {  // event triggered
            auto chIter = this->_fdChannelPtrMap.find(it->fd);  // pollfd can not save a pointer, need a map to save it
            if (chIter == _fdChannelPtrMap.end()) {
                LOG_SYSE("can not find Channel* for fd [%d]", it->fd);
            } else {
                Channel* ch = chIter->second;
                ch->SetrEvents(it->revents);
                channelList->push_back(ch);
                --this->_eventsNum;
            }
        }
    }
}

void PollPoller::ShowPollFd(const struct pollfd* pfd)
{

}
