#include "EventLoop.hh"
#include "EventLoopThreadPool.hh"

using namespace std;
using namespace net;
using namespace base;

int main(int argc, char* argv[])
{
    EventLoop loop;
    EventLoopThreadPool pool;

    pool.start();

    loop.loop();
    return 0;
}