#ifndef __TCP_SERVER_HH__
#define __TCP_SERVER_HH__

#include "EventLoop.hh"
#include "TcpConnection.hh"
#include "Callbacks.hh"
#include "INetAddr.hh"
#include "Log.hh"
#include "Acceptor.hh"
#include "NonCopyable.hh"

#include "EventLoopThread.hh"

#include <string>
#include <map>

using std::string;

typedef std::map<string, net::TcpConnectionPtr> ConnectionMap;

namespace net {

class TcpServer : public base::NonCopyable {
public:
    TcpServer(EventLoop* loop, INetAddr listenAddr, string serverName);
    ~TcpServer();

    // start and stop interface
    void start();
    void stop();

public:
    void setConnectionCallback(const ConnectionCallback& cb)
    {
        this->_connectionCallback = cb;
    }
    void setMessageReadCallback(const MessageReadCallback& cb)
    {
        this->_messageReadCallback = cb;
    }
    void setMessageWriteCompleteCallback(const MessageWriteCompleteCallback& cb)
    {
        this->_messageWriteCompleteCallback = cb;
    }
private:
    void setConnectionCloseCallback(const ConnectionCloseCallback& cb)
    {
        this->_connectionCloseCallback = cb;
    }
private:
    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        this->_newConnectionCallback = cb;
    }

private:
    void HandleNewConnection(int sockFd, INetAddr clientAddr);
    void RemoveConnection(const TcpConnectionPtr& con);   // remove a connection
    void RemoveConnectionInLoop(const TcpConnectionPtr &con); // remove a connection

    void DefaultTcpConnectionHandler(const TcpConnectionPtr& con);

private:
    EventLoop* _eventLoop;  // the eventLoop belonging to
    string _serverName; // the name of this server instance
    INetAddr _listenAddr;   // server addr & port to listen

    // callbacks for TcpConnection
    ConnectionCallback _connectionCallback;
    MessageReadCallback _messageReadCallback;
    MessageWriteCompleteCallback _messageWriteCompleteCallback;
    ConnectionCloseCallback _connectionCloseCallback;

    // callbacks for Acceptor
    NewConnectionCallback _newConnectionCallback;

    // listen socket
    Socket _listenSocket;

    // Acceptror
    Acceptor _acceptor;

    // connections
    ConnectionMap _connectionMap;

    // io loop thread
    EventLoopThread _ioLoopThread;
};

}

#endif