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
    this->_eventLoop->AssertInEventLoop();

    LOG_INFO("conn: [%s] HandleRead begin, time: [%s]", this->_connName.c_str(), readTime.ConvertToString().c_str());
    int saveErrno = 0;
    size_t bytesRead = 0;

    bytesRead = this->_readBuf.ReadFd(this->_sockFd, &saveErrno);
    if (saveErrno != 0) {
        LOG_ERROR("ReadFd failed! errno: [%d]", saveErrno);
        return;
    } else {
        if (bytesRead > 0) {
            LOG_INFO("read [%lu] bytes from fd [%d]", bytesRead, _sockFd);
            if (_messageReadCallback) {
                _messageReadCallback(shared_from_this(), &this->_readBuf, readTime);
            }
        } else if (bytesRead == 0) {    // peer closed the connection, we do the same
            this->HandleClose();
        }
    }
    LOG_INFO("conn: [%s] HandleRead End", this->_connName.c_str());
}

// handle TCP connection close
void TcpConnection::HandleClose()
{
    if (_status == kDisconnected) {
        return;
    }

    // make sure this happens in EventLoop thread
    this->GetLoop()->AssertInEventLoop();
    LOG_INFO("conn: [%s], status: [%s], fd: [%d]", _connName.c_str(), State2String(), _sockFd);

    LOG_INFO("conn: [%s] HandleRead close begin", _connName.c_str());
    // 1. set status to disconnecting
    SetState(kDisconnecting);
    // 2. unregist channel from reactor
    this->_channel->DisableAll();
    this->_channel->Remove();
    // 3. close socket
    // this->_peerSocket.Close();   // done by destructor of 'Socket object'

    SetState(kDisconnected);
    if (_connectionCloseCallback) {
        TcpConnectionPtr guard(shared_from_this()); // hold the object by shared_ptr, we must wait until callback finished
        _connectionCloseCallback(guard);
    }
    LOG_INFO("conn: [%s] HandleRead close end", this->_connName.c_str());
}

// for user call
void TcpConnection::ForceClose()
{
    SetState(kDisconnecting);
    _eventLoop->QueueInLoop(bind(&TcpConnection::ForceCloseInLoop, this));

    // the following code should be done by HandleClose
    /*
    // unregist all event of this fd
    this->_channel->DisableAll();
    this->_channel->Remove();
    // close peer socket too
    this->_peerSocket.Close();
    // inform TcpServer to handle close event
    if (this->_connectionCloseCallback) {
        _connectionCloseCallback(shared_from_this());
    }

    SetState(kDisconnected); */
}

// this will run in EventLoop thread
void TcpConnection::ForceCloseInLoop()
{
    _eventLoop->AssertInEventLoop();
    // _eventLoop->QueueInLoop(bind(&TcpConnection::HandleClose, this)); BUG

    if (this->_status == kDisconnecting || this->_status == kConnected) {
        HandleClose();
    }
}

void TcpConnection::SetState(TcpConnState status)
{
    this->_status = status;
}

const char* TcpConnection::State2String()
{
    switch (_status)
    {
    case kDisconnected:
        return "Disconnected";
        break;
    case kConnected:
        return "Connected";
        break;
    case kConnecting:
        return "Connecting";
        break;
    case kDisconnecting:
        return "Disconnecting";
        break;
    default:
        return "Unknown";
        break;
    }
}