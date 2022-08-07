#include "Poller.hh"
#include "Log.hh"

using namespace net;
using namespace base;

Poller::~Poller()
{
    
}

bool Poller::HasChannel(Channel *channel)
{   
    int fd = channel->GetFd();
    auto it = _channelMap.find(fd);

    if (it != _channelMap.end() && it->second == channel) {
        LOG_DEBUG("found channel for fd [%d]", fd);
        return true;
    } else {
        LOG_WARN("fd [%d] found with no channel!", fd);
    }

    return false;
}