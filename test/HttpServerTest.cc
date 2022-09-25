#include "TcpServer.hh"
#include "EventLoop.hh"
#include "Log.hh"
#include "HexUtils.hh"
#include "StringUtils.hh"

#include <iostream>
#include <sstream>

using namespace std;
using namespace net;
using namespace base;

void ParseHeader(string& header)
{
    vector<string> strVec;

    if (header.find_first_of("\r\n\r\n") == header.npos) {
        cout << "header is not intact!" << endl;
    }

    util::splitString(header, strVec, "\r\n");
    printf("headers:\n");

    for (auto& it : strVec) {
        cout << it << endl;
    }
}

string MakeResponseHeader()
{
    std::stringstream sstream;
    sstream << "HTTP/1.1 200\r\n" \
            << "content-type: application/json; charset=utf-8\r\n" \
            << "server: cloudflare\r\n" \
            << "pragma: no-cache\r\n" \
            << "\r\n";
    return sstream.str();
}

string MakeResponseBody()
{
    std::stringstream sstream;
    sstream << "{\r\n" \
        << "    status: OK\r\n" \
        << "}\r\n";
    return sstream.str();
}

void ReadReqAndSendResp(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp ts)
{
    // LOG_INFO("ReadReqAndSendResp begin! conn: [%s], time: [%s]", conn->GetConnName().c_str(), ts.ConvertToString().c_str());

    string req;
    string resp;

    size_t recvBytes = buf->ReadableBytes();
    LOG_INFO("recv bytes: [%lu]", recvBytes);

    req.assign(buf->ReadBegin(), recvBytes);
    printf("req:\n");
    printf("%s", req.c_str());

    ParseHeader(req);
    resp = MakeResponseHeader();
    resp.append(MakeResponseBody());
    conn->Write(resp);
    conn->ForceClose();

    // buf->RetrieveAll();

    // conn->Write(sendBuf, recvBytes);
    // LOG_INFO("ReadReqAndSendResp end");
}

void NotifyWriteDone(const TcpConnectionPtr& conn)
{
    LOG_INFO("conn [%s] write done!", conn->GetConnName().c_str());
}

int main(int argc, char *argv[])
{
    cout << "TcpServer start!" << endl;
    EventLoop loop;
    INetAddr listenAddr("0.0.0.0", 9091);

    TcpServer myServer(&loop, listenAddr, "myServer");
    myServer.start();
    myServer.setMessageReadCallback(bind(ReadReqAndSendResp, placeholders::_1, placeholders::_2, placeholders::_3));
    myServer.setMessageWriteCompleteCallback(bind(NotifyWriteDone, std::placeholders::_1));

    loop.loop();

    cout << "TcpServer end!" << endl;
    return 0;
}