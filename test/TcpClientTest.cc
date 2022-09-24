#include "EventLoop.hh"
#include "TcpClient.hh"

#include "HexUtils.hh"

using namespace net;
using namespace base;
using namespace std;

void ClientRoutine(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp ts)
{
    LOG_INFO("ClientRoutine begin! conn: [%s], time: [%s]", conn->GetConnName().c_str(), ts.ConvertToString().c_str());

    char sendBuf[512] = {0};

    size_t recvBytes = buf->ReadableBytes();
    LOG_INFO("recv bytes: [%lu]", recvBytes);
    memcpy(sendBuf, buf->ReadBegin(), recvBytes);
    util::hexdump(sendBuf, recvBytes);
    buf->RetrieveAll();

    conn->Write(sendBuf, recvBytes);
    LOG_INFO("ClientRoutine end");
}

int main(int argc, char* argv[])
{
    EventLoop loop;
    INetAddr serverAddr("127.0.0.1", 1088);

    TcpClient client(&loop, serverAddr);
    client.setMessageReadCallback(bind(&ClientRoutine, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    client.connect();

    loop.loop();
    return 0;
}