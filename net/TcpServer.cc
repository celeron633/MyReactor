#include "TcpServer.hh"

using namespace base;
using namespace net;
using namespace std;

TcpServer::TcpServer(EventLoop* loop, INetAddr listenAddr, string serverName) : _eventLoop(loop), _serverName(serverName), _listenAddr(listenAddr), \
    _acceptor(_eventLoop, _listenAddr)
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

    for (auto it = _connectionMap.begin(); it != _connectionMap.end(); ++it) {
        this->RemoveConnection(it->second);
    }

    LOG_WARN("TcpServer::stop() end");
}

void TcpServer::DefaultTcpConnectionHandler(const TcpConnectionPtr& con)
{
    LOG_INFO("welcom new client: [%s]", con->GetConnName().c_str());
}

// handle incoming tcp client
void TcpServer::HandleNewConnection(int sockFd, INetAddr clientAddr)
{
    this->_eventLoop->AssertInEventLoopThread();
    LOG_INFO("HandleNewConnection begin! sockFd: [%d], client info: [%s]", sockFd, clientAddr.GetAddrAndPort().c_str());

    string tcpConnStr = this->_listenAddr.GetAddrAndPort() + "<->" + clientAddr.GetAddrAndPort();
    TcpConnectionPtr con(new TcpConnection(_eventLoop, tcpConnStr, sockFd, clientAddr, _listenAddr));
    con->SetConnectionCallback(bind(&TcpServer::DefaultTcpConnectionHandler, this, std::placeholders::_1));
    con->SetConnectionCloseCallback(bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    con->SetMessageReadCallback(_messageReadCallback);
    con->SetMessageWriteCompleteCallback(_messageWriteCompleteCallback);
    _connectionMap[tcpConnStr] = con;
    /* this->_connectionMap.insert(std::pair<string, TcpConnectionPtr>(tcpConnStr, \
        make_shared<TcpConnection>(TcpConnection(_eventLoop, tcpConnStr, sockFd, clientAddr, _listenAddr)))); */

    _eventLoop->RunInLoopThread(bind(&TcpConnection::ConnectionEstablished, con));

    LOG_INFO("HandleNewConnection end");
    return;
}

// handle connection close
void TcpServer::RemoveConnection(const TcpConnectionPtr& con)
{
    this->_eventLoop->RunInLoopThread(bind(&TcpServer::RemoveConnectionInLoop, this, con));
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& con)
{
    this->_eventLoop->AssertInEventLoopThread();  // make sure this happens in loop thread

    LOG_DEBUG("RemoveConnectionInLoop begin! conn: [%s]", con->GetConnName().c_str());

    auto iter = _connectionMap.find(con->GetConnName());
    if (iter != _connectionMap.end()) {
        this->_connectionMap.erase(iter);
    } else {
        LOG_WARN("can not find conn: [%s] in _connectionMap! maybe it's already erased!", con->GetConnName().c_str());
        return;
    }

    this->_eventLoop->RunInLoopThread(bind(&TcpConnection::ConnectionDestory, con));
    LOG_DEBUG("RemoveConnectionInLoop end");
}
