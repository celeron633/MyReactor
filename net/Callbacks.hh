#ifndef __CALLBACKS_HH__
#define __CALLBACKS_HH__

#include "Timestamp.hh"
#include "INetAddr.hh"
#include "ByteBuffer.hh"

#include <memory>
#include <functional>

using std::shared_ptr;
using base::Timestamp;
using std::function;

using base::ByteBuffer;

namespace base {

    class ByteBuffer;
    class Timestamp;
}

namespace net {

class TcpConnection;

// TcpConnection
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = function<void (const TcpConnectionPtr&)>;
using MessageReadCallback = function<void (const TcpConnectionPtr&, ByteBuffer*, Timestamp)>;
using MessageWriteCompleteCallback = function<void (const TcpConnectionPtr&)>;
using ConnectionCloseCallback = function<void (const TcpConnectionPtr&)>;

// TcpServer
using NewConnectionCallback = function<void (int sockFd, INetAddr clientAddr)>;


// TcpClient

// Channel
using ReadCallback = function<void (Timestamp)>;
using WriteCallback = function<void ()>;
using CloseCallback = function<void ()>;
using ErrorCallback = function<void ()>;




}

#endif