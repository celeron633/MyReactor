#include "TcpServer.hh"
#include "EventLoop.hh"
#include "Log.hh"

#include <iostream>

using namespace std;
using namespace net;
using namespace base;

void ReadClientData(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp ts)
{
    LOG_INFO("ReadClientData begin! conn: [%s], time: [%s]", conn->GetConnName().c_str(), ts.ConvertToString().c_str());

    size_t recvBytes = buf->ReadableBytes();
    LOG_INFO("recv bytes: [%lu]", recvBytes);

    printf("%s\n", buf->ReadBegin());

    LOG_INFO("ReadClientData end");
}

int main(int argc, char *argv[])
{
    cout << "TcpServer start!" << endl;
    EventLoop loop;
    INetAddr listenAddr("0.0.0.0", 9090);

    TcpServer myServer(&loop, listenAddr, "myServer");
    myServer.start();
    myServer.setMessageReadCallback(bind(ReadClientData, placeholders::_1, placeholders::_2, placeholders::_3));

    loop.loop();

    cout << "TcpServer end!" << endl;
    return 0;
}