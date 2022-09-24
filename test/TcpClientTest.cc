#include "EventLoop.hh"
#include "TcpClient.hh"

using namespace net;
using namespace base;
using namespace std;

int main(int argc, char* argv[])
{
    EventLoop loop;
    INetAddr serverAddr("127.0.0.1", 1088);

    TcpClient client(&loop, serverAddr);
    client.connect();

    loop.loop();
    return 0;
}