#include "TcpServer.hh"

using namespace base;
using namespace net;
using namespace std;

TcpServer::TcpServer(EventLoop* loop, INetAddr listenAddr, string serverName) : _eventLoop(loop), _serverName(serverName), _listenAddr(listenAddr), \
    _acceptor(loop, listenAddr)
{
    LOG_INFO("TcpServer: [%s] constructed!", this->_serverName.c_str());
}

TcpServer::~TcpServer()
{
    LOG_INFO("TcpServer: [%s] destructed!", this->_serverName.c_str());
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

    // for ()

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
    con->SetTcpConnectionCloseCallback(bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    con->SetMessageReadCallback(_messageReadCallback);
    con->SetMessageWriteCompleteCallback(_messageWriteCompleteCallback);
    _connectionMap[tcpConnTuple] = con;
    /* this->_connectionMap.insert(std::pair<string, TcpConnectionPtr>(tcpConnTuple, \
        make_shared<TcpConnection>(TcpConnection(_eventLoop, tcpConnTuple, sockFd, clientAddr, _listenAddr)))); */

    _eventLoop->RunInLoop(bind(&TcpConnection::ConnectionEstablished, con));

    LOG_INFO("HandleNewConnection end");
    return;
}

void TcpServer::RemoveConnection(const TcpConnectionPtr& con)
{
    this->_eventLoop->RunInLoop(bind(&TcpServer::RemoveConnectionInLoop, this, con));
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& con)
{
    this->_eventLoop->AssertInEventLoop();  // make sure this happens in loop thread

    auto iter = _connectionMap.find(con->GetConnName());
    if (iter != _connectionMap.end()) {
        this->_connectionMap.erase(iter);
    } else {
        LOG_WARN("can not find conn: [%s] in _connectionMap! maybe it's already erased!", con->GetConnName().c_str());
        return;
    }

    LOG_DEBUG("RemoveConnectionInLoop begin");
    this->_eventLoop->RunInLoop(bind(&TcpConnection::ConnectionDestory, con));
    LOG_DEBUG("RemoveConnectionInLoop end");
}
