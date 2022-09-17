#include "TcpServer.hh"

using namespace base;
using namespace net;
using namespace std;

TcpServer::TcpServer(EventLoop* loop, INetAddr listenAddr, string serverName) : _eventLoop(loop), _serverName(serverName), _listenAddr(listenAddr), \
    _acceptor(loop, listenAddr)
{
    LOG_INFO("TcpServer object constructed!");
}

TcpServer::~TcpServer()
{
    LOG_INFO("TcpServer object destructed!");
}

void TcpServer::start()
{
    LOG_INFO("TcpServer::start begin");
    // get Acceptor ready to listen and accept new connections
    this->_acceptor.setNewConnectionCallback(bind(&TcpServer::HandleNewConnection, this, placeholders::_1, placeholders::_2));
    this->_acceptor.start();
    LOG_INFO("TcpServer::start end");
}

void TcpServer::stop()
{
    LOG_WARN("TcpServer::stop() begin");
    LOG_WARN("TcpServer::stop() end");
}

// handle incoming tcp client
void TcpServer::HandleNewConnection(int sockFd, INetAddr clientAddr)
{
    this->_eventLoop->AssertInEventLoop();
    LOG_INFO("HandleNewConnection begin! sockFd: [%d], client info: [%s]", sockFd, clientAddr.GetAddrAndPort().c_str());

    string tcpConnTuple = this->_listenAddr.GetAddrAndPort() + "<->" + clientAddr.GetAddrAndPort();
    TcpConnectionPtr con(new TcpConnection(_eventLoop, tcpConnTuple, sockFd, clientAddr, _listenAddr));
    con->SetTcpConnectionCallback(_tcpConnectionCallback);
    con->SetTcpConnectionCloseCallback(_tcpConnectionCloseCallback);
    con->SetMessageReadCallback(_messageReadCallback);
    con->SetMessageWriteCompleteCallback(_messageWriteCompleteCallback);
    _connectionMap[tcpConnTuple] = con;
    /* this->_connectionMap.insert(std::pair<string, TcpConnectionPtr>(tcpConnTuple, \
        make_shared<TcpConnection>(TcpConnection(_eventLoop, tcpConnTuple, sockFd, clientAddr, _listenAddr)))); */

    _eventLoop->RunInLoop(bind(&TcpConnection::ConnectionEstablished, con));

    LOG_INFO("HandleNewConnection end");
    return;
}