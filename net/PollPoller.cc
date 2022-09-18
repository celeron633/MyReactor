#include "PollPoller.hh"

using namespace net;
using namespace base;
using namespace std;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop), _eventsNum(0)
{

}

PollPoller::~PollPoller()
{

}