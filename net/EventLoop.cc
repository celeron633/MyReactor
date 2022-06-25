#include "EventLoop.hh"
#include "Poller.hh"
#include "Channel.hh"
#include "Log.hh"

#include <unistd.h>
#include <sys/syscall.h>

using namespace std;
using namespace base;
using namespace net;

EventLoop::EventLoop() : _looping(false), _stopped(true), _tid(CurrentThread::GetThreadId())
{
    LOG_INFO("EventLoop object constructed, tid:[%d]", this->_tid);
}

EventLoop::~EventLoop()
{
    LOG_INFO("EventLoop object destructed!");
}

int main(int argc, char *argv[])
{
    EventLoop loop;
    printf("tid: [%ld]\n", syscall(SYS_gettid));
    return 0;
}