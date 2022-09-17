#ifndef __ACCEPTOR_HH__
#define __ACCEPTOR_HH__

#include "INetAddr.hh"
#include "Callbacks.hh"
#include "Socket.hh"
#include "EventLoop.hh"

namespace net {

class Acceptor {
public:
    Acceptor(EventLoop* loop, INetAddr listenAddr);
    ~Acceptor();

    void start();
    void setNewConnectionCallback(NewConnectionCallback cb)
    {
        _newConnectionCallback = cb;
    }
private:
    void handleRead(Timestamp evTime);
    void listen();
private:
    EventLoop* _eventLoop;
    INetAddr _listenAddr;
    Socket _listenSocket;
    Channel _channel;
    NewConnectionCallback _newConnectionCallback;
};

};

#endif