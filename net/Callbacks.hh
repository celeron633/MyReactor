#ifndef __CALLBACKS_HH__
#define __CALLBACKS_HH__

#include "TcpConnection.hh"
#include "INetAddr.hh"

namespace net {

// TcpConnection && TcpServer
using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void (TcpConnectionPtr)>;
using MessageReadCallback = function<void (TcpConnectionPtr, ByteBuffer*, Timestamp)>;
using MessageWriteCompleteCallback = function<void (TcpConnectionPtr)>;
using TcpConnectionCloseCallback = function<void (TcpConnectionPtr)>;

using NewConnectionCallback = function<void (int sockFd, INetAddr clientAddr)>;
// std::function callbacks definitions

// Channel
using ReadCallback = function<void (Timestamp)>;
using WriteCallback = function<void ()>;
using CloseCallback = function<void ()>;
using ErrorCallback = function<void ()>;

// TcpConnection


}
#endif