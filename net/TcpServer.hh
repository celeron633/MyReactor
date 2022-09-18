#ifndef __TCP_SERVER_HH__
#define __TCP_SERVER_HH__

#include "EventLoop.hh"
#include "TcpConnection.hh"
#include "Callbacks.hh"
#include "INetAddr.hh"
#include "Log.hh"
#include "Acceptor.hh"
#include "NonCopyable.hh"

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
    void setTcpConnectionCallback(TcpConnectionCallback& cb)
    {
        this->_tcpConnectionCallback = cb;
    }
    void setMessageReadCallback(MessageReadCallback& cb)
    {
        this->_messageReadCallback = cb;
    }
    void setMessageWriteCompleteCallback(MessageWriteCompleteCallback& cb)
    {
        this->_messageWriteCompleteCallback = cb;
    }
    void setTcpConnectionCloseCallback(TcpConnectionCloseCallback& cb)
    {
        this->_tcpConnectionCloseCallback = cb;
    }
private:
    void setNewConnectionCallback(NewConnectionCallback& cb)
    {
        this->_newConnectionCallback = cb;
    }

private:
    void HandleNewConnection(int sockFd, INetAddr clientAddr);
    void RemoveConnection(const TcpConnectionPtr& con);   // remove a connection
    void RemoveConnectionInLoop(const TcpConnectionPtr &con); // remove a connection

private:
    EventLoop* _eventLoop;  // the eventLoop belonging to
    string _serverName; // the name of this server instance
    INetAddr _listenAddr;   // server addr & port to listen

    // callbacks for TcpConnection
    TcpConnectionCallback _tcpConnectionCallback;
    MessageReadCallback _messageReadCallback;
    MessageWriteCompleteCallback _messageWriteCompleteCallback;
    TcpConnectionCloseCallback _tcpConnectionCloseCallback;

    // callbacks for Acceptor
    NewConnectionCallback _newConnectionCallback;

    // listen socket
    Socket _listenSocket;

    // Acceptror
    Acceptor _acceptor;

    // connections
    ConnectionMap _connectionMap;
};

}

#endif