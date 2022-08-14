#ifndef __TCP_CONNECTION_HH__
#define __TCP_CONNECTION_HH__

#include "NonCopyable.hh"
#include "Log.hh"
#include "ByteBuffer.hh"
#include "INetAddr.hh"
#include "Channel.hh"
#include "EventLoop.hh"

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

using std::enable_shared_from_this;
using std::shared_ptr;
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

    using TcpConnectionCallback = function<void (TcpConnectionPtr &)>;
    using MessageReadCallback = function<void (TcpConnectionPtr &)>;
    using MessageWriteCompleteCallback = function<void (TcpConnectionPtr &)>;
    using TcpConnectionCloseCallback = function<void (TcpConnectionPtr &)>;

public:
    enum TcpConnState {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
public:
    TcpConnection(EventLoop *eventLoop, string name, int sockFd, const INetAddr &peerAddr, const INetAddr &localAddr);
    ~TcpConnection();


private:
    void HandleRead(Timestamp readTime);
    void HandleWrite();
    void HandleError();
    void HandleClose();

private:
    // one connection has one channel, one loop has one or more channel(connection)
    EventLoop *_eventLoop;
    Channel *_channel;

    // tcp connection fd
    const int _sockFd;

    // connection identifer name
    const string _connName;
    
    // peer and local address info
    const INetAddr _peerAddr;
    const INetAddr _localAddr;

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