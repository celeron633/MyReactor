#include "TcpClient.hh"

using namespace net;
using namespace base;
using namespace std;

TcpClient::TcpClient(EventLoop* loop, INetAddr serverAddr) : 
    _eventLoop(loop), _serverAddr(serverAddr), _connector(loop, _serverAddr), \
    _status(DISCONNECTED)
{
    LOG_DEBUG("TcpClient object constructed! serverAddr: %s", this->_serverAddr.GetAddrAndPort().c_str());
    // set callbacks to Connector object
    this->_connector.setConnectedToServerCallback(bind(&TcpClient::handleConnectSuccess, this, std::placeholders::_1));
    this->_connector.setConnectToServerFailedCallback(bind(&TcpClient::handleConnectError, this));
}

TcpClient::~TcpClient()
{
    LOG_DEBUG("TcpClient object destructed!");
}

void TcpClient::connect()
{
    this->_connector.start();
}

// void TcpClient::setConnectedToServerCallback(const ConnectedToServerCallback& cb)
// {
//     this->_successCallback = cb;
// }

// void TcpClient::setConnectToServerFailedCallback(const ConnectToServerFailedCallback& cb)
// {
//     this->_failedCallback = cb;
// }

void TcpClient::handleConnectError()
{
    LOG_ERROR("connect to server: %s failed!", this->_serverAddr.GetAddrAndPort().c_str());
    _status = DISCONNECTED;
    exit(-1);   // quit here
}

void TcpClient::handleConnectSuccess(int sockFd)
{
    LOG_DEBUG("handleConnectSuccess begin");
    LOG_INFO("connected to server: %s success! fd: [%d]", this->_serverAddr.GetAddrAndPort().c_str(), sockFd);

    INetAddr localAddr;
    INetAddr peerAddr;  // remote server

    sockaddr localSockaddr;
    sockaddr serverSockaddr;

    localSockaddr = Socket::GetLocalSockaddr(sockFd);
    serverSockaddr = Socket::GetRemoteSockaddr(sockFd);

    localAddr.SetSockAddr(localSockaddr);
    peerAddr.SetSockAddr(serverSockaddr);

    string connName = localAddr.GetAddrAndPort() + "<->" + peerAddr.GetAddrAndPort();

    // TcpConnectionPtr con(new TcpConnection(this->_eventLoop, connName, sockFd, peerAddr, localAddr));
    this->_tcpConnectionPtr.reset(new TcpConnection(this->_eventLoop, connName, sockFd, peerAddr, localAddr));
    _tcpConnectionPtr->SetConnectionCallback(this->_connectionCallback);
    _tcpConnectionPtr->SetMessageReadCallback(_messageReadCallback);
    _tcpConnectionPtr->SetMessageWriteCompleteCallback(_messageWriteCompleteCallback);
    _tcpConnectionPtr->SetConnectionCloseCallback(_connectionCloseCallback);
    // _tcpConnectionPtr->SetConnectionCloseCallback(bind(&TcpClient::removeConnection, this, _tcpConnectionPtr));
    // WARN: above line caused a shared_ptr leak, we should use a placeholders, let the caller release itself from shared_form_this(), when we
    // use _tcpConnectionPtr, this will be copyed into the bind closure, so it leaked
    _tcpConnectionPtr->SetConnectionCloseCallback(bind(&TcpClient::removeConnection, this, placeholders::_1));

    // regist read event for TcpConnection
    this->_eventLoop->RunInLoopThread(bind(&TcpConnection::ConnectionEstablished, _tcpConnectionPtr));

    LOG_DEBUG("handleConnectSuccess end");
}

void TcpClient::setConnectionCallback(const ConnectionCallback& cb)
{
    this->_connectionCallback = cb;
}

void TcpClient::setMessageReadCallback(const MessageReadCallback& cb)
{
    this->_messageReadCallback = cb;
}

void TcpClient::setMessageWriteCompleteCallback(const MessageWriteCompleteCallback& cb)
{
    this->_messageWriteCompleteCallback = cb;
}

// void TcpClient::setConnectionCloseCallback(const ConnectionCloseCallback& cb)
// {
//     this->_connectionCloseCallback = cb;
// }

void TcpClient::removeConnection(const TcpConnectionPtr& con)
{
    this->_eventLoop->RunInLoopThread(bind(&TcpClient::removeConnectionInLoop, this, con));
}

void TcpClient::removeConnectionInLoop(const TcpConnectionPtr& con)
{
    this->_eventLoop->AssertInEventLoopThread();

    LOG_INFO("removeConnectionInLoop begin!");
    // set a guard
    TcpConnectionPtr guard(con);
    // run this to remove channel, also run _connectionCallback in TcpConnection
    this->_eventLoop->RunInLoopThread(bind(&TcpConnection::ConnectionDestory, guard));

    // release the hold TcpConnectionPtr
    this->_tcpConnectionPtr.reset();
    LOG_INFO("removeConnectionInLoop end!");
}