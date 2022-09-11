#include "TcpConnection.hh"

using namespace base;
using namespace net;
using namespace std;

TcpConnection::TcpConnection(EventLoop* eventLoop, string name, int sockFd, const INetAddr& peerAddr, const INetAddr& localAddr) \
    : _eventLoop(eventLoop), _channel(new Channel(_eventLoop, _sockFd)), _sockFd(sockFd), _connName(name), \
    _peerAddr(peerAddr), _localAddr(localAddr){
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
    LOG_INFO("TcpConnection object: [%s] destructed!", _connName.c_str());
}

// handler for event 'READ'
void TcpConnection::HandleRead(Timestamp readTime)
{
    LOG_INFO("HandleRead begin @%s", readTime.ConvertToString().c_str());
    int saveErrno = 0;
    size_t bytesRead = 0;

    bytesRead = this->_readBuf.ReadFd(this->_sockFd, &saveErrno);
    if (saveErrno != 0) {
        LOG_ERROR("ReadFd failed! errno: [%d]", saveErrno);
        return;
    } else {
        LOG_INFO("read [%lu] bytes from fd [%d]", bytesRead, _sockFd);
        if (_messageReadCallback) {
            _messageReadCallback(shared_from_this(), readTime, &this->_readBuf);
        }
    }
    LOG_INFO("HandleRead End");
}