#ifndef __CONNECTOR_HH__
#define __CONNECTOR_HH__

#include "EventLoop.hh"
#include "Callbacks.hh"
#include "INetAddr.hh"
#include "Socket.hh"
#include "Log.hh"

#include <memory>
#include <atomic>

using std::unique_ptr;

namespace net {

class Connector {

enum ConnectorStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};
public:
    Connector(EventLoop *loop, INetAddr serverAddr);
    ~Connector();

    void start();
    int getStatus();
    void setConnectToServerSuccessCallback(const ConnectToServerSuccessCallback& cb);
    void setConnectToServerFailedCallback(const ConnectToServerFailedCallback& cb);
private:
    void startInLoop();
    void connect();
    void connecting(int sockFd);
    void retry();
    void setStatus(int stat);

    void socketIsReadyToWrite(int sockFd);

    void defaultConnectToServerSuccessCallback(int sockFd);
private:
    void removeChannel();
private:
    EventLoop* _loop;
    INetAddr _serverAddr;
    // int _sockFd;
    std::atomic_int _status;

    unique_ptr<Channel> _connectorChannel;
    ConnectToServerSuccessCallback _successCallback;
    ConnectToServerFailedCallback _failedCallback;
};

};

#endif