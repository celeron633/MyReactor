#include "TcpConnection.hh"

using namespace base;
using namespace net;
using namespace std;

TcpConnection::TcpConnection(EventLoop* eventLoop, string name, int sockFd, const INetAddr& peerAddr, const INetAddr& localAddr) \
    : _eventLoop(eventLoop), _sockFd(sockFd), _channel(new Channel(_eventLoop, _sockFd)), _connName(name), \
    _peerAddr(peerAddr), _localAddr(localAddr), _peerSocket(_sockFd), _status(kConnecting)
{
    LOG_INFO("TcpConnection object constructed! connName: [%s], peerFd: [%d]", _connName.c_str(), _sockFd);
    
    // set callbacks for channel for this TcpConnection object
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
    // make sure this happens in loop thread
    this->_eventLoop->AssertInEventLoopThread();

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
            LOG_INFO("peer close! we close too!");
            this->HandleClose();
        }
    }
    if (!(bytesRead == 0)) {
        LOG_INFO("conn: [%s] HandleRead End", this->_connName.c_str());
    }
}

// handle TCP connection close
void TcpConnection::HandleClose()
{
    if (_status == kDisconnected) {
        LOG_WARN("call HandleClose while status is kDisconnected!");
        return;
    }

    // make sure this happens in EventLoop thread
    _eventLoop->AssertInEventLoopThread();
    LOG_INFO("conn: [%s], status: [%s], fd: [%d]", _connName.c_str(), State2String(), _sockFd);

    LOG_INFO("conn: [%s] HandleRead close begin", _connName.c_str());
    // 1. set status to disconnecting
    SetState(kDisconnected);
    // 2. unregist channel from reactor
    this->_channel->DisableAll();
    // this->_channel->Remove();
    // 3. close socket
    // this->_peerSocket.Close();   // done by destructor of 'Socket object'

    // let upper class know this connection is going to destory
    if (_connectionCallback) {
        TcpConnectionPtr guard(shared_from_this());
        _connectionCallback(guard);
    }

    // SetState(kDisconnecting);
    if (_connectionCloseCallback) {
        TcpConnectionPtr guard(shared_from_this()); // hold the object by shared_ptr, we must wait until callback finished
        _connectionCloseCallback(guard);
    }
    // LOG_INFO("conn: [%s] HandleRead close end", this->_connName.c_str());    // this will cause a invalid read
}

void TcpConnection::HandleError()
{
    LOG_WARN("channel called HandleError!");
    HandleClose();
}

void TcpConnection::Write(const string& str)
{
    Write(str.data(), str.length());
}

void TcpConnection::Write(const char* buf, size_t len)
{
    this->_eventLoop->RunInLoopThread(bind(&TcpConnection::WriteInLoop, this, buf, len));
}

void TcpConnection::WriteInLoop(const char* buf, size_t len)
{
    // make sure this happens in loop thread, not in working thread
    _eventLoop->AssertInEventLoopThread();

    if (this->_status == kDisconnected) {
        LOG_WARN("call WriteInLoop while TcpConnection is kDisconnected!");
        return;
    }

    LOG_DEBUG("WriteInLoop begin!");
    // if the channel is not interested in 'Writable', just send to client/server with ::write
    if (!_channel->IsWriteEnabled()) {
        ssize_t writeLen = ::write(this->_sockFd, buf, len);
        size_t remainingLen = len - writeLen;

        if (writeLen == (ssize_t)len) {
            LOG_DEBUG("call ::write() with full-success, writeLen: [%lu], reqLen: [%ld], remainingLen: [%lu]", writeLen, len, remainingLen);
            if (this->_messageWriteCompleteCallback) {
                // _messageWriteCompleteCallback(shared_from_this());
                _eventLoop->QueueInLoop(std::bind(_messageWriteCompleteCallback, shared_from_this()));
            }
        } else if (writeLen < (ssize_t)len) {
            LOG_DEBUG("call ::write() with part-success, writeLen: [%lu], reqLen: [%ld], remainingLen: [%lu]", writeLen, len, remainingLen);
            // 余下部分加入buffer, 关注写事件再发送
            this->_writeBuf.Append(buf + writeLen, remainingLen);
            // 关注写事件
            this->_channel->EnableWrite();
        } else if (writeLen < 0 && (errno != EAGAIN || errno != EWOULDBLOCK)) {
            perror("call ::write() failed");
            LOG_ERROR("WriteInLoop failed!");
            // TODO: call errorCallBack
        }
    } else {
        LOG_DEBUG("channel IsWriteEnabled() returns true, still some data in writeBuffer, will append to writeBuffer!");
        this->_writeBuf.Append(buf, len);
    }
    LOG_DEBUG("WriteInLoop end!");
}

void TcpConnection::HandleWrite()
{
    LOG_INFO("HandleWrite begin");
    // make sure in loop thread
    this->_eventLoop->AssertInEventLoopThread();

    if (this->_status == kDisconnected) {
        LOG_WARN("call HandleWrite while TcpConnection is kDisconnected!");
        return;
    }

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
            // this->_messageWriteCompleteCallback(shared_from_this());
            _eventLoop->QueueInLoop(bind(this->_messageWriteCompleteCallback, shared_from_this()));
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
    _eventLoop->AssertInEventLoopThread();
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
    this->_eventLoop->AssertInEventLoopThread();

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
    if (this->_connectionCallback) {
        _connectionCallback(shared_from_this());
    }

    LOG_DEBUG("ConnectionEstablishedInLoop end! TcpConnection object state is [%s]", State2String());
}

void TcpConnection::ConnectionDestory()
{
    _eventLoop->RunInLoopThread(bind(&TcpConnection::ConnectionDestoryInLoop, this));
}

void TcpConnection::ConnectionDestoryInLoop()
{
    this->_eventLoop->AssertInEventLoopThread();

    LOG_DEBUG("ConnectionDestoryInLoop begin! conn status: [%s]", State2String());

   /* if (this->_status != kConnected && this->_status != kDisconnecting) {
        LOG_WARN("call ConnectionDestoryInLoop while status is not kConnected or kDisconnecting, status: [%s]", State2String());
        return;
    } */

    if (this->_status == kConnected) {
        this->SetState(kDisconnected);
        TcpConnectionPtr guard(this);
        if (_connectionCallback) {
            _connectionCallback(guard);
        }
    }

    // this->_channel->DisableAll();
    this->_channel->Remove();

    // this->SetState(kDisconnected);

    // run callback
    /* if (this->_connectionCloseCallback) {
        TcpConnectionPtr guard(shared_from_this());
        _connectionCloseCallback(guard);
    } */

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
    case kConnected:
        return "Connected";
    case kConnecting:
        return "Connecting";
    case kDisconnecting:
        return "Disconnecting";
    default:
        return "Unknown";
    }
}

void TcpConnection::ShutdownWrite(void)
{
    this->_eventLoop->RunInLoopThread(bind(&TcpConnection::ShutdownWriteInLoop, this));
}

void TcpConnection::ShutdownWriteInLoop()
{
    this->_eventLoop->AssertInEventLoopThread();

    LOG_DEBUG("ShutdownWriteInLoop begin");
    this->_peerSocket.ShutdownWrite();
    if (this->_status == kConnected) {
        this->SetState(kDisconnecting);
    }
    LOG_DEBUG("ShutdownWriteInLoop end");
}