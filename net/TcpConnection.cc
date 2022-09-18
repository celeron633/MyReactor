#include "TcpConnection.hh"

using namespace base;
using namespace net;
using namespace std;

TcpConnection::TcpConnection(EventLoop* eventLoop, string name, int sockFd, const INetAddr& peerAddr, const INetAddr& localAddr) \
    : _eventLoop(eventLoop), _channel(new Channel(_eventLoop, _sockFd)), _sockFd(sockFd), _connName(name), \
    _peerAddr(peerAddr), _localAddr(localAddr), _status(kConnecting){
    LOG_INFO("TcpConnection object constructed! connName: [%s]", _connName.c_str());
    
    // set callbacks for channel
    _channel->SetReadCallback(bind(&TcpConnection::HandleRead, this, std::placeholders::_1));
    _channel->SetWriteCallback(bind(&TcpConnection::HandleWrite, this));
    _channel->SetErrorCallback(bind(&TcpConnection::HandleError, this));
    _channel->SetCloseCallback(bind(&TcpConnection::HandleClose, this));

    
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection : [%s] destructed!", _connName.c_str());
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
        LOG_WARN("call HandleClose while status is kDisconnected!");
        return;
    }

    // make sure this happens in EventLoop thread
    _eventLoop->AssertInEventLoop();
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

ssize_t TcpConnection::WriteInLoop(const char* buf, size_t len)
{
    // make sure this happens in loop thread, not in working thread
    _eventLoop->AssertInEventLoop();

    LOG_DEBUG("WriteInLoop begin!");
    // if the channel is not interested in 'Writable', just send to client/server with ::write
    if (!_channel->IsWriteEnabled()) {
        ssize_t writeLen = ::write(this->_sockFd, buf, len);
        size_t remainingLen = len - writeLen;

        if (writeLen == (ssize_t)len) {
            LOG_DEBUG("call ::write() with full-success, writeLen: [%lu], reqLen: [%ld], remainingLen: [%lu]", writeLen, len, remainingLen);
            if (this->_messageWriteCompleteCallback) {
                _messageWriteCompleteCallback(shared_from_this());
            }
            return writeLen;
        } else if (writeLen < (ssize_t)len) {
            LOG_DEBUG("call ::write() with part-success, writeLen: [%lu], reqLen: [%ld], remainingLen: [%lu]", writeLen, len, remainingLen);
            // 余下部分加入buffer, 关注写事件再发送
            this->_writeBuf.Append(buf + writeLen, remainingLen);
            // 关注写事件
            this->_channel->EnableWrite();
            return writeLen;
        } else if (writeLen < 0 && (errno != EAGAIN || errno != EWOULDBLOCK)) {
            perror("call ::write() failed");
            LOG_ERROR("WriteInLoop failed!");
            // TODO: call errorCallBack
            return -1;
        }
    } else {
        LOG_DEBUG("channel IsWriteEnabled() returns true, still some data in writeBuffer, will append to writeBuffer!");
        this->_writeBuf.Append(buf, len);
        return 0;
    }
    LOG_DEBUG("WriteInLoop end!");
    return 0;
}

void TcpConnection::HandleWrite()
{
    LOG_INFO("HandleWrite begin");
    // make sure in loop thread
    this->_eventLoop->AssertInEventLoop();

    if (!_channel->IsWriteEnabled()) {
        LOG_WARN("call HandleWrite while channel is not writeEnabled!");
        return;
    }

    // 写缓存已经写完了
    if (this->_writeBuf.ReadableBytes() == 0) {
        LOG_DEBUG("writeBuf is empty! channel will DisableWrite!");
        this->_channel->DisableWrite();
        return;
    }

    // 写缓存还余下多少
    size_t bufRemainingSize = _writeBuf.ReadableBytes();
    const char* bufAddr = _writeBuf.ReadBegin();
    
    // 调用写socket
    ssize_t actualWriteSize = ::write(this->_sockFd, bufAddr, bufRemainingSize);
    if (actualWriteSize < 0 && (errno != EAGAIN || errno != EWOULDBLOCK)) { // write失败
        perror("call ::write() failed");
        LOG_ERROR("HandleWrite write failed!");
        return;
    }

    if (actualWriteSize == (ssize_t)bufRemainingSize) {  // 写缓存全部发送完了
        this->_writeBuf.RetrieveAll();
        LOG_DEBUG("::write send all writeBuf data! will DisableWrite!");
        this->_channel->DisableWrite(); // 全部发送完了不再关注写事件
        if (this->_messageWriteCompleteCallback) {
            this->_messageWriteCompleteCallback(shared_from_this());
        }
    } else if (actualWriteSize < (ssize_t)bufRemainingSize) {    // 写缓存发送出去了部分
        LOG_DEBUG("::write send [%ld] bytes from writeBuf", actualWriteSize);
        this->_writeBuf.Retrieve(actualWriteSize);
    }
    LOG_INFO("HandleWrite End");

    return;
}

// for upper layer call
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

// for upper layer call
void TcpConnection::ConnectionEstablished()
{
    _eventLoop->QueueInLoop(bind(&TcpConnection::ConnectionEstablishedInLoop, this));
}

void TcpConnection::ConnectionEstablishedInLoop()
{
    LOG_DEBUG("ConnectionEstablishedInLoop begin");
    // make sure all things happened in LoopThread
    this->_eventLoop->AssertInEventLoop();

    // only do following step when state is kConnecting
    if (_status != kConnecting) {
        LOG_WARN("call ConnectionEstablishedInLoop while state is not kConnecting!");
        return;
    }

    // regist channel with read event
    this->_channel->EnableRead();

    // set status as 'connected'
    SetState(kConnected);

    // run callback
    if (this->_connectionOKCallback) {
        _connectionOKCallback(shared_from_this());
    }

    LOG_DEBUG("ConnectionEstablishedInLoop end! TcpConnection object state is [%s]", State2String());
}

void TcpConnection::ConnectionDestory()
{
    _eventLoop->RunInLoop(bind(&TcpConnection::ConnectionDestoryInLoop, this));
}

void TcpConnection::ConnectionDestoryInLoop()
{
    this->_eventLoop->AssertInEventLoop();

    LOG_DEBUG("ConnectionDestoryInLoop begin!");

    if (this->_status != kConnected || this->_status != kDisconnecting) {
        LOG_WARN("call ConnectionDestoryInLoop while status is not kConnected or kDisconnecting");
        return;
    }

    this->SetState(kDisconnecting);

    this->_channel->DisableAll();
    this->_channel->Remove();

    this->SetState(kDisconnected);

    // run callback
    if (this->_connectionCloseCallback) {
        TcpConnectionPtr guard(shared_from_this());
        _connectionCloseCallback(guard);
    }

    LOG_DEBUG("ConnectionDestoryInLoop end! TcpConnection object state is [%s]", State2String());
    return;
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