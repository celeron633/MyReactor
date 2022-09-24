#ifndef __TCP_CLIENT_HH__
#define __TCP_CLIENT_HH__

#include "EventLoop.hh"
#include "TcpConnection.hh"
#include "Connector.hh"
#include "Log.hh"

namespace net {

class TcpClient {
public:
    TcpClient(EventLoop* loop, INetAddr serverAddr);
    ~TcpClient();

    void connect();
    // void setConnectedToServerCallback(const ConnectedToServerCallback& cb);
    // void setConnectToServerFailedCallback(const ConnectToServerFailedCallback& cb);

    void handleConnectError();
    void handleConnectSuccess(int sockFd);

    void setConnectionCallback(const ConnectionCallback& cb);
    void setMessageReadCallback(const MessageReadCallback& cb);
    void setMessageWriteCompleteCallback(const MessageWriteCompleteCallback& cb);
    void setConnectionCloseCallback(const ConnectionCloseCallback& cb);
private:
    EventLoop* _eventLoop;
    INetAddr _serverAddr;
    Connector _connector;

    TcpConnectionPtr _tcpConnectionPtr;
    unique_ptr<Channel> _channel;

    // for Connector
    ConnectedToServerCallback _successCallback;
    ConnectToServerFailedCallback _failedCallback;

    // callbacks for TcpConnection
    ConnectionCallback _connectionCallback;
    MessageReadCallback _messageReadCallback;
    MessageWriteCompleteCallback _messageWriteCompleteCallback;
    ConnectionCloseCallback _connectionCloseCallback;
};

};

#endif