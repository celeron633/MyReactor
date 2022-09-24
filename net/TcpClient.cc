#include "TcpClient.hh"

using namespace net;
using namespace base;
using namespace std;

TcpClient::TcpClient(EventLoop* loop, INetAddr serverAddr) : 
    _eventLoop(loop), _serverAddr(serverAddr), _connector(loop, _serverAddr)
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

    this->_tcpConnectionPtr.reset(new TcpConnection(this->_eventLoop, connName, sockFd, peerAddr, localAddr));
    _tcpConnectionPtr->SetConnectionCallback(this->_connectionCallback);
    _tcpConnectionPtr->SetMessageReadCallback(_messageReadCallback);
    _tcpConnectionPtr->SetMessageWriteCompleteCallback(_messageWriteCompleteCallback);
    _tcpConnectionPtr->SetConnectionCloseCallback(_connectionCloseCallback);

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

void TcpClient::setConnectionCloseCallback(const ConnectionCloseCallback& cb)
{
    this->_connectionCloseCallback = cb;
}