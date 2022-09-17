#ifndef __TCP_CONNECTION_HH__
#define __TCP_CONNECTION_HH__

#include "NonCopyable.hh"
#include "Log.hh"
#include "ByteBuffer.hh"
#include "INetAddr.hh"
#include "Channel.hh"
#include "EventLoop.hh"
#include "Socket.hh"

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

using std::enable_shared_from_this;
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::function;
using std::bind;
using std::atomic_int;

using base::NonCopyable;
using base::ByteBuffer;
using base::Logger;

namespace net {

// enable 'enable_shared_from_this' to get a shared this pointer
class TcpConnection : public enable_shared_from_this<TcpConnection>, public NonCopyable {
    using TcpConnectionPtr = shared_ptr<TcpConnection>;

    using TcpConnectionCallback = function<void (TcpConnectionPtr)>;
    using MessageReadCallback = function<void (TcpConnectionPtr, ByteBuffer*, Timestamp)>;
    using MessageWriteCompleteCallback = function<void (TcpConnectionPtr)>;
    using TcpConnectionCloseCallback = function<void (TcpConnectionPtr)>;

public:
    enum TcpConnState {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
public:
    //  peerAddr && localAddr: tcp four tuple
    TcpConnection(EventLoop* eventLoop, string name, int sockFd, const INetAddr& peerAddr, const INetAddr& localAddr);
    ~TcpConnection();

    int GetConnStatus()
    {
        return _status;
    }

    string GetConnName()
    {
        return _connName;
    }

    EventLoop* GetEventLoop(void)
    {
        return _eventLoop;
    }

    // 关闭socket写
    void ShutdownWrite()
    {
        this->_channel->DisableWrite();
        if (::shutdown(this->_sockFd, SHUT_WR) < 0) {
            perror("socket shutdown");
        }
    }

    // write data to tcp connection
    ssize_t Write(const char* buf, size_t len);
    ssize_t Write(const string& str);
    ssize_t WriteInLoop(const char* buf, size_t len);
    // force close the connection
    void ForceClose();
    void ForceCloseInLoop();

    // for 'TcpServer' & 'TcpClient'
    void ConnectionEstablished();
    void ConnectionEstablishedInLoop();
    void ConnectionDestory();
    void ConnectionDestoryInLoop();
public:
    void SetTcpConnectionCallback(TcpConnectionCallback cb)
    {
        this->_connectionOKCallback = cb;
    }
    void SetMessageReadCallback(MessageReadCallback cb)
    {
        this->_messageReadCallback = cb;
    }
    void SetMessageWriteCompleteCallback(MessageWriteCompleteCallback cb)
    {
        this->_messageWriteCompleteCallback = cb;
    }
    void SetTcpConnectionCloseCallback(TcpConnectionCloseCallback cb)
    {
        this->_connectionCloseCallback = cb;
    }

private:
    void SetState(TcpConnState);
    const char* State2String();
private:
    void HandleRead(Timestamp readTime);
    void HandleWrite();
    void HandleError();
    void HandleClose();

private:
    // one connection has one channel, one loop has one or more channel(connection)
    EventLoop* _eventLoop;
    unique_ptr<net::Channel> _channel;  // use smart pointer for channel

    // tcp connection fd
    const int _sockFd;

    // connection identifer name
    const string _connName;
    
    // peer and local address info
    const INetAddr _peerAddr;
    const INetAddr _localAddr;

    // peer Socket object
    Socket _peerSocket;

    // connection status
    atomic_int _status;

    // user-level buffers for non-blocking I/O
    ByteBuffer _readBuf;
    ByteBuffer _writeBuf;

    // callbacks for TcpConnection
    TcpConnectionCallback _connectionOKCallback;
    MessageReadCallback _messageReadCallback;
    MessageWriteCompleteCallback _messageWriteCompleteCallback;
    TcpConnectionCloseCallback _connectionCloseCallback;
};

};

#endif