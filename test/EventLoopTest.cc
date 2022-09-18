#include "EventLoop.hh"

using namespace net;
using namespace std;

int main(int argc, char *argv[])
{
    EventLoop loop;
    printf("tid: [%ld]\n", syscall(SYS_gettid));
    loop.AssertInEventLoopThread();
    loop.loop();
    return 0;
}
