#include "Poller.hh"
#include "Log.hh"

using namespace net;
using namespace base;

bool Poller::HasChannel(Channel *channel)
{
    auto it = _channelMap.find(channel->GetFd());

    if (it != _channelMap.end() && it->second == channel) {
        LOG_DEBUG("found channel for fd [%d]", channel->GetFd());
        return true;
    } else {
        LOG_WARN("fd [%d] found no channel!", channel->GetFd());
    }

    return false;
}