#include "TcpConnection.hh"

using namespace base;
using namespace net;
using namespace std;

TcpConnection::TcpConnection(EventLoop *eventLoop, string name, int sockFd, const INetAddr &peerAddr, const INetAddr &localAddr) \
    : _eventLoop(eventLoop), _channel(new Channel(_eventLoop, _sockFd)), _sockFd(sockFd), _connName(name), \
    _peerAddr(peerAddr), _localAddr(localAddr) {
    LOG_INFO("TcpConnection object constructed! connName: [%s]", _connName.c_str());
    
    // set callbacks for channel
    _channel->SetReadCallback(bind(&TcpConnection::HandleRead, this, std::placeholders::_1));
    _channel->SetWriteCallback(bind(&TcpConnection::HandleWrite, this));
    _channel->SetErrorCallback(bind(&TcpConnection::HandleError, this));
    _channel->SetCloseCallback(bind(&TcpConnection::HandleClose, this));

    _status = kConnecting;
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection object [%s] destructed!", _connName.c_str());

    delete _channel;
}

