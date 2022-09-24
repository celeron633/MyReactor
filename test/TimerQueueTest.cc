#include "EventLoop.hh"

using namespace std;
using namespace base;
using namespace net;

void TestCallback(void)
{
    Timestamp now;
    cout << "[" << now.ConvertToString().c_str() << "] " << "hello world" << endl;
}

int main(int argc, char* argv[])
{
    EventLoop loop;

    Timestamp now;
    now += 6000;
    loop.runAt(bind(&TestCallback), now);

    loop.loop();
    return 0;
}