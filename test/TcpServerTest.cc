#include "TcpServer.hh"
#include "EventLoop.hh"

#include <iostream>

using namespace std;
using namespace net;
using namespace base;

int main(int argc, char *argv[])
{
    cout << "TcpServer start!" << endl;
    EventLoop loop;
    INetAddr listenAddr("0.0.0.0", 9090);

    TcpServer myServer(&loop, listenAddr, "myServer");
    myServer.start();

    loop.loop();

    cout << "TcpServer end!" << endl;
    return 0;
}